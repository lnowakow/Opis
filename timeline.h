#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVector>

#include "qcustomplot.h"
#include "seeker.h"
#include "timeseriesdata.h"

class Timeline : public QCustomPlot
{
    Q_OBJECT
public:
    Seeker *s;

    Timeline(QWidget *parent = nullptr);
    ~Timeline();
    bool videoLoaded() { return *_video_loaded; };
    void setVideoLoaded(bool state) { *_video_loaded = state; };
    bool isDataLoaded() { return *_data_loaded; };
    void setDataLoaded(bool state) { *_data_loaded = state; };
    void openCSVFile(const QString &filename);
    void setSeekerPosition(float position);

public slots:
    void newVideoLoaded(const QMediaPlayer& playerInfo);
    void updateTimeline(TimeSeriesData* const& data);
    void isVideoPlaying(const bool& isPlaying);

signals:
    void requestData();
    void updateWorkspacePlot(TimeSeriesData* const& data, float pos, float area_lo, float area_uo);

private:
    TimeSeriesData *_data;

    qint64 *_video_duration;
    qint64 *_video_position;
    qint64 *_data_duration;
    qint64 *_data_offset;

    bool *_video_loaded;
    bool *_video_playing;
    bool *_data_loaded;

    void SetupBackground();

};

#endif // TIMELINE_H
