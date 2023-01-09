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
using LabelMap = QMap<int, QString>;

class TimeSeriesData : public QObject
{
    Q_OBJECT
public:
    TimeSeriesData(QCustomPlot *parent);
    TimeSeriesData(QCustomPlot *parent, const QString &filename);
    ~TimeSeriesData();

    CSVData *csv_data;
    LabelMap *lbls;
    QVector<double> time;
    QVector<double> label_value;
    QVector<QColor> label_color;
    QVector<double> offset_time;
    SeekerPosition *offset;

    const QVector<QColor> lbl_colors = {
        Qt::black,                          // [0]
        Qt::red,                            // [1]
        Qt::blue,                           // [2]
        Qt::green,                          // [3]
        QColorConstants::Svg::magenta,      // [4]
        QColorConstants::Svg::orange,       // [5]
        Qt::yellow,                         // [6]
        QColorConstants::Svg::cornsilk,     // [7]
        QColorConstants::Svg::darksalmon    // [8]
    };

public slots:
    void updateData();
    void onMousePress(QMouseEvent *event);
    void onMouseMove(QMouseEvent *event);
    void onMouseRelease(QMouseEvent *event);
    void workspaceSelection(const int& begin, const int& end);
    void applyClassLabel(const int& label_idx);
    void exportData(const QString& filename);

signals:
    void replotNow(TimeSeriesData* const &data);
    void onDoneCSVExport(const QString& filename);

private:
    QCPLayer *_csv_layer;
    QCPItemRect *_area;
    float _begin;
    float _end;

    QPoint _qp_area_origin;
    bool _area_selected;

    int _label_selection_begin;
    int _label_selection_end;
    bool _label_area_selected;
};

#endif // TIMESERIESDATA_H
