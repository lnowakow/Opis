#include "timeline.h"

#include "qcustomplot.h"
#include "qcpcolorcurve.h"

#include <QVector>
#include <QBoxLayout>
#include <QFileDialog>
#include <algorithm>

Timeline::Timeline(QWidget *parent) :
    s(new Seeker(this)),
    _video_duration(new qint64(250)),
    _video_position(new qint64(0)),
    _data_duration(new qint64(0)),
    _data_offset(new qint64(0)),
    _video_loaded(new bool(false)),
    _data_loaded(new bool(false)),
    _data(new TimeSeriesData(this))
{

    this->setMinimumHeight(80);
    this->setMaximumHeight(80);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    connect(s, &Seeker::replotNow, this, [&](void) {
        this->replot();
    });
    //connect(s, &Seeker::replotNow, this, &Timeline::updateTimeline);

    SetupBackground();

}

Timeline::~Timeline()
{
    delete _data;

    delete _video_duration;
    delete _video_position;
    delete _data_duration;
    delete _data_offset;

    delete _video_loaded;
    delete _data_loaded;
}

void Timeline::openCSVFile(const QString &filename)
{
    qDebug() << "Opening CSV File";
    TimeSeriesData *tmp = new TimeSeriesData(this, filename);
    delete _data;
    _data = tmp;
    *_data_loaded = true;
    *_data_duration = _data->time.size();
    connect(_data, &TimeSeriesData::replotNow, this, &Timeline::updateTimeline);
    connect(s, &Seeker::replotNow, this, [&](void) { qDebug() << "here"; _data->updateData(); });

    qDebug() << "Length of data: " << _data->time.size();
    //updateTimeline();
}

void Timeline::setSeekerPosition(float position)
{
    s->position->set(position);
}

void Timeline::newVideoLoaded(const QMediaPlayer& playerInfo)
{
    qDebug() << "New Video Uploaded!";
    *_video_duration = playerInfo.duration();
    *_video_position = playerInfo.position();

    *_video_loaded = true;

    qDebug() << "Duration: " << *_video_duration << ", Position: " << *_video_position << ", Loaded: " << *_video_loaded;

    this->xAxis->setRange(0, *_video_duration);
}

void Timeline::SetupBackground()
{
    // loop for each sensor value
    //this->addGraph();
    //this->graph(0)->setPen(QPen(Qt::blue));

    // fake data
//    QVector<double> x(*_video_duration), y0(*_video_duration);
//    for (int i = 0; i < *_video_duration; ++i) {
//        x[i] = i;
//        y0[i] = qExp(-i/150.0) * qCos(i/10.0);
//    }


    // config axis labels
    this->xAxis->setTickLabels(true);
    this->yAxis->setTickLabels(true);
    this->yAxis->setVisible(true);
    // config right and top axis to show labels but no ticks
    this->xAxis2->setVisible(false);
    this->xAxis2->setTickLabels(false);
    this->yAxis2->setVisible(true);
    this->yAxis2->setTickLabels(true);
    // make left and bottom axes always transfer their ranges to right and top axes:
    //connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    //connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graph
    //this->graph(0)->addData(x, y0);
    this->xAxis->setRange(0,*_video_duration);
    this->yAxis->setRange(0,1);
    this->setInteractions(QCP::iSelectItems);

    this->xAxis->rescale(false);
    this->yAxis->rescale(false);
    this->xAxis2->rescale(false);
    this->yAxis2->rescale(true);

    this->replot();
}

void Timeline::updateTimeline(TimeSeriesData* const &data)
{
    if (*_data_loaded) {
        // Data Related
        this->clearGraphs();
        this->clearPlottables();

        const int num_layers = this->layerCount()-1;

        if (data != nullptr) {
            for (const auto& feature : *data->csv_data) {
                QCPColorCurve *curve = new QCPColorCurve(this->xAxis, this->yAxis2);
                curve->setData(data->offset_time, feature, data->label_color);
                curve->setLineStyle(QCPCurve::lsLine);
                curve->setScatterStyle(QCPScatterStyle::ssDot);
                curve->setScatterSkip(0);
                curve->setVisible(true);
                curve->setLayer(this->layer(num_layers));
                curve->rescaleValueAxis(true);
                curve->setLayer("main");

//                this->addGraph(this->xAxis, this->yAxis2);
//                const int graph_idx = this->graphCount()-1;
//                this->graph(graph_idx)->setPen(QPen(Qt::black));
//                this->graph(graph_idx)->addData(data->offset_time, feature);
//                this->graph(graph_idx)->rescaleValueAxis(true);

            }
            emit updateWorkspacePlot(data, s->position->get(), s->getAreaLO(), s->getAreaUO());
        }
    }
    this->replot();

}
