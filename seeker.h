#ifndef SEEKER_H
#define SEEKER_H

#include "qcustomplot.h"
#include "seekerposition.h"

#define SCROLL_SENS 1000

class Seeker : public QObject
{
    Q_OBJECT;
public:
    Seeker(QCustomPlot *parent);
    ~Seeker();
    SeekerPosition *position;
    float getAreaLO() { return _area_lo; }
    float getAreaUO() { return _area_uo; }

public slots:
    void selectionChanged(bool selected);
    void videoChanged(const QCPRange &range);

private slots:
    void updateArea(float pos);
    void onMousePress(QMouseEvent *event);
    void onMouseMove(QMouseEvent *event);
    void onMouseRelease(QMouseEvent *event);
    void onMouseWheel(QWheelEvent *event);

signals:
    void replotNow();

private:
    QCPLayer *_seeker_layer;

    QCPItemStraightLine *_handle;
    float _min_position;
    float _max_position;

    QCPItemRect *_area;
    float _area_max_width;
    float _area_lo;
    float _area_uo;

    QPoint _qp_area_origin;
    bool _area_selected;

};

#endif // SEEKER_H
