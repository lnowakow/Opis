#ifndef WORKSPACEPLOT_H
#define WORKSPACEPLOT_H
/*
 * Plot the shaded region of the timeline.
 *  - TimeSeriesData data
 *  - timeline Seeker._area_lo (xAxis lower limit)
 *  - timeline Seeker._area_uo (xAxis upper limit)
 *
 *
 */
#include <QObject>
#include <QVector>

#include "qcustomplot.h"
#include "timeseriesdata.h"
#include "timeline.h"

#define FIXED_HEIGHT 200

class WorkspacePlot : public QCustomPlot
{
    Q_OBJECT;
public:
    WorkspacePlot(Timeline *parent);

public slots:
    void updateWorkspacePlot(TimeSeriesData* const& data, float pos, float area_lo, float area_uo);
    void selectionChanged(const QCPDataSelection& selection);

signals:
    void workspaceSelection(const int& begin, const int& end);

private:
    QCPItemStraightLine *_handle;
    int lo;
    int uo;

    template <class T>
    int getClosestIndex(QVector<double> const &vec, T val);
    template <class T>
    QVector<T> slice(QVector<T> const &v, int m, int n);
};

#endif // WORKSPACEPLOT_H
