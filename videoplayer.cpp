#include "videoplayer.h"

VideoPlayer::VideoPlayer(QWidget* parent) :
    QWidget(parent)
{
    player = new QMediaPlayer(this);
    vw = new QVideoWidget();
    player->setVideoOutput(vw);


}
