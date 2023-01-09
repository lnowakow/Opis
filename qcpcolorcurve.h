#ifndef QCPCOLORCURVE_H
#define QCPCOLORCURVE_H

#include <QVector>
#include "qcustomplot.h"

class QCPColorCurve : public QCPCurve
{
public:
    QCPColorCurve(QCPAxis *keyAxis, QCPAxis *valueAxis);
    virtual ~QCPColorCurve();
    void setData(const QVector<double> & keys, const QVector<double> & values, const QVector<QColor> & colors);
protected:
    virtual void drawCurveLine(QCPPainter *painter, const QVector<QPointF> &lines) const;
    virtual void drawScatterPlot(QCPPainter *painter, const QVector<QPointF> &points, const QCPScatterStyle &style) const;
private:
    QVector<QColor> colors_;
};

#endif // QCPCOLORCURVE_H
