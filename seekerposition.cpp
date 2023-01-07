#include "seekerposition.h"

SeekerPosition::SeekerPosition()
{

}

SeekerPosition::SeekerPosition(float val) :
    _position(val)
{

}

void SeekerPosition::set(float val)
{
    _position = val;
    emit positionChanged(_position);
}

float SeekerPosition::get()
{
    return _position;
}

void SeekerPosition::setPosition(qint64 pos)
{
    set(pos);
}
