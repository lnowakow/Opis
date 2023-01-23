#include <algorithm>
#include "workspaceplot.h"
#include "qcpcolorcurve.h"

WorkspacePlot::WorkspacePlot(Timeline *parent) :
    _handle(new QCPItemStraightLine(this)),
    lo(0),
    uo(0)
{
    this->addLayer("seekerLayer", this->layer("main"), QCustomPlot::limAbove);
    QCPLayer *seeker_layer = this->layer("seekerLayer");

    this->setMinimumHeight(FIXED_HEIGHT);
    this->setMaximumHeight(FIXED_HEIGHT);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // config axis labels
    this->xAxis->setTickLabels(true);
    this->xAxis->setVisible(true);
    this->yAxis->setTickLabels(true);
    this->yAxis->setVisible(true);
    // config right and top axis to show labels but no ticks
    this->xAxis2->setVisible(false);
    this->xAxis2->setTickLabels(false);
    this->yAxis2->setVisible(false);
    this->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graph
    //this->graph(0)->addData(x, y0);
    this->xAxis->setRange(0,1);
    this->yAxis->setRange(0,1);
    this->setInteractions(QCP::iSelectPlottables | QCP::iMultiSelect);
    this->setSelectionRectMode(QCP::srmSelect);

    // handle init
    _handle->point1->setCoords(0.5,0);
    _handle->point2->setCoords(0.5,1);
    QPen v_seeker_pen;
    v_seeker_pen.setColor(Qt::red);
    v_seeker_pen.setWidth(1);
    _handle->setPen(v_seeker_pen);
    _handle->setLayer(seeker_layer);

    this->replot();
}

void WorkspacePlot::updateWorkspacePlot(TimeSeriesData* const &data, float pos, float area_lo, float area_uo)
{
    this->clearGraphs();
    this->clearPlottables();

//    qDebug() << "pos-area_lo: " << pos-area_lo << ", pos+area_uo: " << pos+area_uo;

    if (pos+area_lo < 0) {
//        qDebug() << "CASE 5";
        lo = 0;
        uo = 0;
    }
    else if (pos-area_lo < 0) {
//        qDebug() << "CASE 2";
        lo = 0;
        uo = getClosestIndex(data->offset_time, pos+area_uo);
    }
    else if (pos+area_uo > data->offset_time[data->offset_time.size()-1]) {
//        qDebug() << "CASE 3";
        lo = getClosestIndex(data->offset_time, pos-area_lo);
        uo = data->offset_time.size()-1;
    } else if (pos-area_lo > data->offset_time[data->offset_time.size()-1]) {
//        qDebug() << "CASE 4";
        lo = data->offset_time.size()-1;
        uo = data->offset_time.size()-1;
    } else {
//        qDebug() << "CASE 1";
        lo = getClosestIndex(data->offset_time, pos-area_lo);
        uo = getClosestIndex(data->offset_time, pos+area_uo);
    }

//    qDebug() << "LO: " << lo << ", UO: " << uo;
//    qDebug() << "LO[" << lo << "] val: " << data->offset_time[lo] << ", UO[" << uo << "] val: " << data->offset_time[uo];

    if (lo == -1 || uo == -1) {
        qDebug() << "Value not found in vector";
        return;
    }

    const int num_layers = this->layerCount()-1;

    if (data != nullptr) {
        qDebug() << "Number of features: " << data->csv_data->keys().size();
        int counter = 0;
        for (const auto& feature : *data->csv_data) {
            // Set coloring
            QCPColorCurve *curve = new QCPColorCurve(this->xAxis, this->yAxis);
            curve->setData(slice(data->offset_time,lo,uo), slice(feature, lo, uo), slice(data->label_color, lo, uo));
            curve->setLineStyle(QCPCurve::lsLine);
            curve->setScatterStyle(QCPScatterStyle::ssDot);
            curve->setScatterSkip(0);
            curve->setSelectable(QCP::stDataRange);
            curve->setVisible(true);
            curve->setLayer(this->layer(num_layers));
            curve->setLayer("main");
            curve->setName("wsPlot"+QString::number(counter));

            QCPSelectionDecorator *selDec = new QCPSelectionDecorator();
            selDec->setBrush(QBrush(QColor(50,50,50,80)));
            curve->setSelectionDecorator(selDec);
            this->xAxis->rescale(true);
            this->yAxis->rescale(true);

            connect(curve, SIGNAL(selectionChanged(QCPDataSelection)), this, SLOT(selectionChanged(QCPDataSelection)));
            counter++;
        }
    }

    _handle->point1->setCoords(pos,0);
    _handle->point2->setCoords(pos,1);

    this->replot();

}

void WorkspacePlot::selectionChanged(const QCPDataSelection &selection)
{
    qDebug() << "Begin idx: " << selection.span().begin() << ", End idx: " << selection.span().end();
    if (!selection.isEmpty()) {
        emit workspaceSelection(selection.span().begin()+lo, selection.span().end()+lo);
    } else {
        emit workspaceSelection(-1,-1);
    }
}

template <class T>
int WorkspacePlot::getClosestIndex(QVector<double> const &vec, T val)
{
    auto it = std::lower_bound(vec.begin(), vec.end(), val);

    // If element was found
    if (it != vec.end())
    {
        // calculating the index
        // of K
        return it - vec.begin();
    }
    else if (it == vec.end()) {
        return it - vec.begin() - 1;
    } else {
        return -1;
    }
}

template <class T>
QVector<T> WorkspacePlot::slice(QVector<T> const &v, int m, int n)
{
    auto first = v.cbegin() + m;
    auto last = v.cbegin() + n + 1;

    QVector<T> vec(first, last);
    return vec;
}
