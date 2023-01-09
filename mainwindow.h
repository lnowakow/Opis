#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>
#include <QString>
#include "timeline.h"
#include "timeseriesdata.h"
#include "workspaceplot.h"

using ClassLabels = QMap<QString, int>;

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
    void applyClassLabel(const int& label_idx);
    void exportData(const QString& filename);

private slots:
    void on_actionOpenVideo_triggered();
    void on_actionOpenCSV_triggered();

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

    void on_actionEdit_Class_Labels_triggered();

    void onDoneCSVExport(const QString& filename);

private:
    Ui::MainWindow *_ui;
    // Media Control Buttons
    QPushButton *stepBack;
    QPushButton *playPause;
    QPushButton *stepForward;
    QPushButton *stop;

    QDoubleSpinBox *_offsetSpinBox;
    QComboBox *_classComboBox;

    QMediaPlayer *_player;
    QVideoWidget *_vw;
    Timeline *_t;
    WorkspacePlot *_plot;
    TimeSeriesData *_data;

    ClassLabels *_lbl;

    bool _isVideoPlaying;

};
#endif // MAINWINDOW_H
