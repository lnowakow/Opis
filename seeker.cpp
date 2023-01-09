#include "seeker.h"

Seeker::Seeker(QCustomPlot *parent) :
    _handle(new QCPItemStraightLine(parent)),
    position(new SeekerPosition(0.0f)),
    _area_max_width(10000.0f),
    _area_lo(0.0f),
    _area_uo(5000.0f),
    _area(new QCPItemRect(parent))
{
    parent->addLayer("seekerLayer", parent->layer("main"), QCustomPlot::limAbove);
    _seeker_layer = parent->layer("seekerLayer");

    // area init
    _area->setPen(QPen(Qt::transparent));
    _area->setBrush(QBrush(QColor(50,50,50,90)));
    for (auto pos : _area->positions()) {
        pos->setAxes(parent->xAxis, parent->yAxis);
        pos->setType(QCPItemPosition::ptPlotCoords);
    }
    _area->topLeft->setCoords(position->get()-_area_lo, 1);
    _area->bottomRight->setCoords(position->get()+_area_uo, 0 );
    _area->setSelectable(true);
    _area->setSelectedBrush(QBrush(Qt::gray));
    _area->setLayer(_seeker_layer);

    // handle init
    _handle->point1->setCoords(position->get(),0);
    _handle->point2->setCoords(position->get(),1);
    QPen v_seeker_pen;
    v_seeker_pen.setColor(Qt::red);
    v_seeker_pen.setWidth(1);
    _handle->setPen(v_seeker_pen);
    _handle->setLayer(_seeker_layer);

    // connections
    connect(_area, &QCPAbstractItem::selectionChanged, this, &Seeker::selectionChanged);
    connect(parent->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(videoChanged(QCPRange)));
    connect(position, &SeekerPosition::positionChanged, this, &Seeker::updateArea);
    connect(parent, &QCustomPlot::mousePress, this, &Seeker::onMousePress);
    connect(parent, &QCustomPlot::mouseMove, this, &Seeker::onMouseMove);
    connect(parent, &QCustomPlot::mouseRelease, this, &Seeker::onMouseRelease);
    connect(parent, &QCustomPlot::mouseWheel, this, &Seeker::onMouseWheel);
}

Seeker::~Seeker()
{

}

void Seeker::selectionChanged(bool selected)
{
    selected ? qDebug() << "Grey area selected" : qDebug() << "Grey area unselected";
    //_area_selected = selected;


}

void Seeker::videoChanged(const QCPRange &range)
{
    _min_position = range.lower;
    _max_position = range.upper;

}

void Seeker::updateArea(float pos)
{
    // position is less than beginning of video
    if (pos < _min_position) {
        position->set(_min_position);
        return;
    }
    // position is more than video length
    if (pos > _max_position) {
        position->set(_max_position);
        return;
    }

    // window width extends into negative area
    if (pos - _area_lo <= _min_position) {
        _area_lo = pos - _min_position;
    } else if (_area_lo < _area_uo) {
        _area_lo = _area_uo;
    }
    // window width extends past the video length
    if (pos + _area_uo >= _max_position) {
        _area_uo = _max_position - pos;
    } else if (_area_uo < _area_lo) {
        _area_uo = _area_lo;
    }

    // scrolling limits
    if (_area_lo > _area_max_width) {
        _area_lo = _area_max_width;
    } else if (_area_lo < 0) {
        _area_lo = 0;
    }
    if (_area_uo > _area_max_width) {
        _area_uo = _area_max_width;
    } else if (_area_uo < 0) {
        _area_uo = 0;
    }

    //qDebug() << "Position: " << position->get() << "LO: " << _area_lo << "UO: " << _area_uo;
    _handle->point1->setCoords(pos,0);
    _handle->point2->setCoords(pos,1);
    _area->topLeft->setCoords(pos-_area_lo, 1);
    _area->bottomRight->setCoords(pos+_area_uo, 0);

    emit replotNow();
}

void Seeker::onMousePress(QMouseEvent* event) {
    qDebug() << "Mouse Pressed";
    if (event->button() == Qt::LeftButton &&
            float(event->pos().x()) > _area->positions()[0]->pixelPosition().x() &&
            float(event->pos().x()) < _area->positions()[1]->pixelPosition().x()) {
        _area_selected = true;
        _qp_area_origin = event->pos();
    } else if (event->button() == Qt::RightButton) {
        position->set(_area->positions()[0]->keyAxis()->pixelToCoord(event->pos().x()));
    }

}

void Seeker::onMouseMove(QMouseEvent *event)
{
    if (_area_selected) {
        position->set(_area->positions()[0]->keyAxis()->pixelToCoord(event->pos().x()));
    }
}

void Seeker::onMouseRelease(QMouseEvent *event)
{
    qDebug() << "Mouse Released";
    _area_selected = false;
}

void Seeker::onMouseWheel(QWheelEvent *event)
{
    if(event->angleDelta().y() > 0) { // up Wheel
        _area_lo += SCROLL_SENS;
        _area_uo += SCROLL_SENS;
    } else if(event->angleDelta().y() < 0) {//down Wheel
        _area_lo -= SCROLL_SENS;
        _area_uo -= SCROLL_SENS;
    }

    updateArea(position->get());
}
