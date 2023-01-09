#include "timeseriesdata.h"
#include <algorithm>

TimeSeriesData::TimeSeriesData(QCustomPlot *parent) :
    csv_data(new CSVData()),
    lbls(new LabelMap()),
    offset(new SeekerPosition(0.0f)),
    _area(new QCPItemRect(parent)),
    _area_selected(false),
    _label_selection_begin(0),
    _label_selection_end(0),
    _label_area_selected(false)
{

}

TimeSeriesData::TimeSeriesData(QCustomPlot *parent, const QString &filename) :
    csv_data(new CSVData()),
    lbls(new LabelMap()),
    offset(new SeekerPosition(0.0f)),
    _area(new QCPItemRect(parent)),
    _area_selected(false),
    _label_selection_begin(0),
    _label_selection_end(0),
    _label_area_selected(false)
{

    std::ifstream csv_file(filename.toStdString());
        if (!csv_file.is_open()) throw std::runtime_error("Could not open file!");

        // helper
        std::string line;
        std::string feature_name;
        double val;

        // populate feature names vector
        std::getline(csv_file, line);
        std::stringstream feature_row(line);
        QVector<QString> features;

        while(std::getline(feature_row, feature_name, ',')) {
            features.push_back(QString::fromStdString(feature_name));
            qDebug() << "Feature Name from CSV: " << QString::fromStdString(feature_name);
        }

        qDebug() << "Num features: " << features.size();

        QVector<QVector<double>*> tmp(features.size());
        for (auto &vec : tmp) {
            vec = new QVector<double>();
            vec->reserve(100000);
        }


        while (std::getline(csv_file, line)) {
            std::stringstream ss(line);
            int colIdx = 0;
            while (ss >> val) {
                tmp[colIdx%features.size()]->push_back(val);
                if(ss.peek() == ',') ss.ignore();
                colIdx++;
            }
        }

        for (int i = 0; i < features.size(); ++i) {
            csv_data->insert(features[i], *tmp[i]);
        }

        if (!csv_data->contains("label")) {
            label_value = QVector<double>(tmp[0]->size(), 0);
        } else {
            label_value = QVector<double>((*csv_data)["label"].begin(), (*csv_data)["label"].end());
            csv_data->remove("label");
        }
        label_color.reserve(label_value.size());
        for (int i = 0; i < label_value.size(); ++i) {
            label_color.push_back(lbl_colors[int(label_value[i])]);
        }

        time = QVector<double>((*csv_data)["time"].begin(), (*csv_data)["time"].end());
        csv_data->remove("time");
        offset_time = time;
        for (auto &e : offset_time) {
            e += offset->get();
        }

        _begin = *time.begin();
        _end = *(time.end()-1);
        qDebug() << "start val: " << _begin << "end val: " << _end;

        csv_file.close();

        parent->addLayer("csvLayer", 0, QCustomPlot::limAbove);
        _csv_layer = parent->layer("csvLayer");

        // area init
        //_area->setPen(QPen(Qt::transparent));
        _area->setBrush(QBrush(Qt::blue));
        for (auto pos : _area->positions()) {
            pos->setAxes(parent->xAxis, parent->yAxis);
            pos->setType(QCPItemPosition::ptPlotCoords);
        }
        _area->topLeft->setCoords(_begin, 0.1);
        _area->bottomRight->setCoords(_end, 0 );
        _area->setSelectable(false);
        _area->setLayer(_csv_layer);

        // connections
//        connect(_area, &QCPAbstractItem::selectionChanged, this, &Seeker::selectionChanged);
//        connect(parent->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(videoChanged(QCPRange)));
        connect(offset, &SeekerPosition::positionChanged, this, &TimeSeriesData::updateData);
        connect(parent, &QCustomPlot::mousePress, this, &TimeSeriesData::onMousePress);
        connect(parent, &QCustomPlot::mouseMove, this, &TimeSeriesData::onMouseMove);
        connect(parent, &QCustomPlot::mouseRelease, this, &TimeSeriesData::onMouseRelease);
}

TimeSeriesData::~TimeSeriesData()
{
    delete csv_data;
}

void TimeSeriesData::updateData()
{
    offset_time = time;
    for (auto &e : offset_time) {
        e += offset->get();
    }

    _begin = *offset_time.begin();
    _end = *(offset_time.end()-1);

    _area->topLeft->setCoords(_begin, 0.1);
    _area->bottomRight->setCoords(_end, 0 );
    emit replotNow(this);
}

void TimeSeriesData::onMousePress(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton &&
            float(event->pos().x()) > _area->positions()[0]->pixelPosition().x() &&
            float(event->pos().x()) < _area->positions()[1]->pixelPosition().x() &&
            float(event->pos().y()) > _area->positions()[0]->pixelPosition().y()  &&
            float(event->pos().y()) <= _area->positions()[1]->pixelPosition().y()) {
        _area_selected = true;
        _qp_area_origin = event->pos();
        qDebug() << "TimeSeriesData: Selected!";
    }
}

void TimeSeriesData::onMouseMove(QMouseEvent *event)
{
    if (_area_selected) {
        float delta = _area->positions()[0]->keyAxis()->pixelToCoord(event->pos().x())
                - _area->positions()[0]->keyAxis()->pixelToCoord(_qp_area_origin.x());
        qDebug() << "Delta: " << delta << " Offset: " << offset->get();

        offset->set(offset->get() + delta);
        qDebug() << "Offset after delta: " << offset->get();
        _qp_area_origin = event->pos();
    }
}

void TimeSeriesData::onMouseRelease(QMouseEvent *event)
{
    qDebug() << "TimeSeriesData: Mouse Released";
    _area_selected = false;
}

void TimeSeriesData::workspaceSelection(const int &begin, const int &end)
{
    qDebug() << "selected data range: " << begin << ", " << end;
    if (begin == -1 && end == -1) {
        _label_selection_begin = 0;
        _label_selection_end = 0;
        _label_area_selected = false;
    } else {
        _label_selection_begin = begin;
        _label_selection_end = end;
        _label_area_selected = true;
    }
}

void TimeSeriesData::applyClassLabel(const int &label_idx)
{
    qDebug() << "applying class label";
    if (_label_area_selected) {
        std::fill(label_value.begin()+_label_selection_begin, label_value.begin()+_label_selection_end, label_idx);
        std::fill(label_color.begin()+_label_selection_begin, label_color.begin()+_label_selection_end, lbl_colors[label_idx]);

        emit replotNow(this);
    }
}

void TimeSeriesData::exportData(const QString& filename)
{
    qDebug() << "CSV File will save under: " << filename;
    std::ofstream out;
    out.open(filename.toStdString());
    //QDebug out(QtDebugMsg);
    // Column Names First
    out << "\"time\",";
    CSVData::Iterator iter;
    for (iter = csv_data->begin(); iter != csv_data->end(); ++iter) {
        out << iter.key().toStdString() << ",";
    }
    out << "\"label\"\n";

    // Data
    for (int i = 0; i < time.size(); ++i) {
        out << time.value(i) << ",";
        for (const auto& feature : *csv_data) {
            out << feature[i] << ",";
        }
        out << label_value[i] << "\n";
    }

    out.close();
    emit onDoneCSVExport(filename);
}

