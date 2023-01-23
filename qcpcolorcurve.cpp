#include "qcpcolorcurve.h"

QCPColorCurve::QCPColorCurve(QCPAxis *keyAxis, QCPAxis *valueAxis) : QCPCurve(keyAxis, valueAxis) {}

QCPColorCurve::~QCPColorCurve(){ }

void QCPColorCurve::setData(const QVector<double> & keys, const QVector<double> & values, const QVector<QColor> & colors){
    if (values.size() != colors.size()) return;
    colors_ = colors;
    QCPCurve::setData(keys, values);
}

void QCPColorCurve::drawCurveLine(QCPPainter *painter, const QVector<QPointF> &lines) const
{
    applyDefaultAntialiasingHint(painter);
    int nLines = lines.size();
    for (int i = 0; i < nLines; ++i) {
        int offset_i = this->selection().span().begin()+i;
        if (offset_i < colors_.size()) {
            painter->setPen(colors_[offset_i]);

        }
        if (i < nLines-1) {
            drawPolyline(painter, QVector<QPointF>(lines.begin()+i, lines.begin()+i+2));
        }
    }
}

void QCPColorCurve::drawScatterPlot(QCPPainter * painter, const QVector<QPointF> & points, const QCPScatterStyle & style) const {
    applyScattersAntialiasingHint(painter);
    int nPoints = points.size();
    for (int i = 0; i < nPoints; ++i)
        if (!qIsNaN(points.at(i).x()) && !qIsNaN(points.at(i).y())){
            painter->setPen(colors_[i]);
            style.drawShape(painter, points.at(i));
        }
}
