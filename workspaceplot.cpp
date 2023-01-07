#include <algorithm>
#include "workspaceplot.h"

WorkspacePlot::WorkspacePlot(Timeline *parent)
{
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
    this->setInteractions(QCP::iSelectItems);
    this->replot();
}

void WorkspacePlot::updateWorkspacePlot(TimeSeriesData* const &data, float pos, float area_lo, float area_uo)
{
    int num_graphs = this->graphCount();
    if (num_graphs > 1) {
        int i = 0;
        while (i < num_graphs) {
            this->removeGraph(i);
            i++;
        }
    }


    int lo;
    int uo;

    qDebug() << "pos-area_lo: " << pos-area_lo << ", pos+area_uo: " << pos+area_uo;

    if (pos+area_lo < 0) {
        qDebug() << "CASE 5";
        lo = 0;
        uo = 0;
    }
    else if (pos-area_lo < 0) {
        qDebug() << "CASE 2";
        lo = 0;
        uo = getClosestIndex(data->offset_time, pos+area_uo);
    }
    else if (pos+area_uo > data->offset_time[data->offset_time.size()-1]) {
        qDebug() << "CASE 3";
        lo = getClosestIndex(data->offset_time, pos-area_lo);
        uo = data->offset_time.size()-1;
    } else if (pos-area_lo > data->offset_time[data->offset_time.size()-1]) {
        qDebug() << "CASE 4";
        lo = data->offset_time.size()-1;
        uo = data->offset_time.size()-1;
    } else {
        qDebug() << "CASE 1";
        lo = getClosestIndex(data->offset_time, pos-area_lo);
        uo = getClosestIndex(data->offset_time, pos+area_uo);
    }

    qDebug() << "LO: " << lo << ", UO: " << uo;
    qDebug() << "LO[" << lo << "] val: " << data->offset_time[lo] << ", UO[" << uo << "] val: " << data->offset_time[uo];

    if (lo == -1 || uo == -1) {
        qDebug() << "Value not found in vector";
        return;
    }

    if (data != nullptr) {
        for (const auto& feature : *data->csv_data) {
            this->addGraph(this->xAxis, this->yAxis);
            const int graph_idx = this->graphCount()-1;
            this->graph(graph_idx)->setPen(QPen(Qt::black));
            this->graph(graph_idx)->addData(slice(data->offset_time,lo,uo), slice(feature, lo, uo));
//            this->graph(graph_idx)->rescaleKeyAxis(true);
//            this->graph(graph_idx)->rescaleValueAxis(true);
            this->xAxis->rescale(true);
            this->yAxis->rescale(true);
        }
    }
    this->replot();
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
