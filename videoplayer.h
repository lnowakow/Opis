#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>



class VideoPlayer : public QWidget
{
    Q_OBJECT;
public:
    VideoPlayer(QWidget* parent);

private:
    QMediaPlayer* player;
    QVideoWidget* vw;

};

#endif // VIDEOPLAYER_H
