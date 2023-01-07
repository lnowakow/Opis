#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>
#include <QString>
#include "timeline.h"
#include "timeseriesdata.h"
#include "workspaceplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void newVideoLoaded(const QMediaPlayer& playerInfo);

private slots:
    void on_actionOpenVideo_triggered();
    void on_actionOpenCSV_triggered();
    void on_actionPlay_triggered();
    void on_actionPause_triggered();
//    void on_actionStop_triggered();

    void onActionStepBackTriggered();
    void onActionPlayPauseTriggered();
    void onActionStepForwardTriggered();
    void onActionStopTriggered();

    void onActionOffsetSpinBoxValueChanged(double d);
    void onActionClassComboBoxCurrentTextChanged(const QString &text);

    void onActionShiftSelectionLeftTriggered();
    void onActionShiftSelectionRightTriggered();
    void onActionGrowSelectionLeftTriggered();
    void onActionShrinkSelectionLeftTriggered();
    void onActionGrowSelectionRightTriggered();
    void onActionShrinkSelectionRightTriggered();

    void onActionApplyClassLabelTriggered();
    void onActionExportDataTriggered();



private:
    Ui::MainWindow *_ui;
    QMediaPlayer *_player;
    QVideoWidget *_vw;
    Timeline *_t;
    WorkspacePlot *_plot;
    TimeSeriesData *_data;

    bool _isVideoPlaying;

};
#endif // MAINWINDOW_H
