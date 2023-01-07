#ifndef TIMESERIESDATA_H
#define TIMESERIESDATA_H

#include <fstream>
#include <sstream>

#include <QObject>
#include <QVector>
#include <QMap>

#include "qcustomplot.h"
#include "seekerposition.h"

using CSVData = QMap<QString, QVector<double>>;

class TimeSeriesData : public QObject
{
    Q_OBJECT
public:
    TimeSeriesData(QCustomPlot *parent);
    TimeSeriesData(QCustomPlot *parent, const QString &filename);
    ~TimeSeriesData();

    CSVData *csv_data;
    QVector<double> time;
    QVector<double> offset_time;
    SeekerPosition *offset;

public slots:
    void updateData();
    void onMousePress(QMouseEvent *event);
    void onMouseMove(QMouseEvent *event);
    void onMouseRelease(QMouseEvent *event);

signals:
    void replotNow(TimeSeriesData* const &data);

private:
    QCPLayer *_csv_layer;
    QCPItemRect *_area;
    float _begin;
    float _end;

    QPoint _qp_area_origin;
    bool _area_selected;
};

#endif // TIMESERIESDATA_H
