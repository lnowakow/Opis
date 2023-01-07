#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QBoxLayout>
#include <QFrame>
#include <QSplitter>
#include <QSpacerItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindow)
    , _player(new QMediaPlayer(this))
    , _vw(new QVideoWidget(this))
    , _t(new Timeline(this))
    , _plot(new WorkspacePlot(_t))
    , _data(new TimeSeriesData(_t))
    , _isVideoPlaying(false)
{
    _ui->setupUi(this);

//    // Parent Widget
//    QWidget *centralWidget = new QWidget;

//    // The needed layouts
//    QVBoxLayout *videoLayout = new QVBoxLayout;
//    QVBoxLayout *plotLayout = new QVBoxLayout;
//    QVBoxLayout *timelineLayout = new QVBoxLayout;
//    QVBoxLayout *centralLayout = new QVBoxLayout;

//    // Connecting the pieces:
//    videoLayout->addWidget(_vw);
//    videoFrame->setLayout(videoLayout);
//    videoFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

//    plotLayout->addWidget(_plot);
//    plotFrame->setLayout(plotLayout);

//    timelineLayout->addWidget(_t);
//    timelineFrame->setLayout(timelineLayout);


//    centralLayout->addWidget(videoFrame);
//    centralLayout->addWidget(plotFrame);
//    centralLayout->addWidget(timelineFrame);
//    centralWidget->setLayout(centralLayout);
//    setCentralWidget(centralWidget);
////////////////////////////////////////////////////////////////////////////////////////////////////

    // Main Video/Data Workspace
    QFrame *visualFrame = new QFrame(this);
    // Video Content
    QFrame *videoFrame = new QFrame(this);
    _player->setVideoOutput(_vw);

    // Detailed plot Content
    QFrame *plotFrame = new QFrame(this);
    _plot->setMinimumHeight(200);
    _plot->setMaximumHeight(200);
    _plot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // Timeline Content
    QFrame *timelineFrame = new QFrame(this);


    // SideBar
    QFrame *buttonsFrame = new QFrame(this);
    QLabel *mediaControlsLabel = new QLabel("Media Controls", this);
    mediaControlsLabel->setAlignment(Qt::AlignCenter);

    QFrame *mediaButtonsFrame = new QFrame(this);
    QPushButton *stepBack = new QPushButton("Step Back", this);
    QPushButton *playPause = new QPushButton("Play/Pause", this);
    QPushButton *stepForward = new QPushButton("Step Forward", this);
    QPushButton *stop = new QPushButton("Stop", this);

    QLabel *offsetSpinBoxLabel = new QLabel("Data Position", this);
    offsetSpinBoxLabel->setAlignment(Qt::AlignCenter);
    QDoubleSpinBox *offsetSpinBox = new QDoubleSpinBox(this);
    offsetSpinBox->setRange(-1.79769E+308, 1.79769E+308);
    offsetSpinBox->setDecimals(4);

    QLabel *classComboBoxLabel = new QLabel("Class Label", this);
    classComboBoxLabel->setAlignment(Qt::AlignCenter);
    QComboBox *classComboBox = new QComboBox(this);

    QFrame *selectionEditFrame = new QFrame(this);
    QPushButton *shiftSelectionLeft = new QPushButton("Shift Left", this);
    QPushButton *shiftSelectionRight = new QPushButton("Shift Right", this);
    QPushButton *growSelectionLeft = new QPushButton("Grow Left", this);
    QPushButton *shrinkSelectionLeft = new QPushButton("Shrink Left", this);
    QPushButton *growSelectionRight = new QPushButton("Grow Right", this);
    QPushButton *shrinkSelectionRight = new QPushButton("Shrink Right", this);


    QPushButton *applyClassLabel = new QPushButton("Apply Class Label", this);
    QPushButton *exportData = new QPushButton("Export CSV", this);

    // Layouts for Everything
    QVBoxLayout *videoLayout = new QVBoxLayout;
    QVBoxLayout *plotLayout = new QVBoxLayout;
    QVBoxLayout *timelineLayout = new QVBoxLayout;
    QVBoxLayout *videoDataWorspaceLayout = new QVBoxLayout;
    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    QHBoxLayout *mediaButtonsLayout = new QHBoxLayout;
    QGridLayout *selectionEditLayout = new QGridLayout;

    // videoDataWorkspace
    videoLayout->addWidget(_vw);
    videoFrame->setLayout(videoLayout);
    videoFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    plotLayout->addWidget(_plot);
    plotFrame->setLayout(plotLayout);

    timelineLayout->addWidget(_t);
    timelineFrame->setLayout(timelineLayout);

    videoDataWorspaceLayout->addWidget(videoFrame);
    videoDataWorspaceLayout->addWidget(plotFrame);
    videoDataWorspaceLayout->addWidget(timelineFrame);

    visualFrame->setLayout(videoDataWorspaceLayout);

    // mediaButtons
    mediaButtonsLayout->addWidget(stepBack);
    mediaButtonsLayout->addWidget(playPause);
    mediaButtonsLayout->addWidget(stepForward);
    mediaButtonsLayout->addWidget(stop);

    mediaButtonsFrame->setLayout(mediaButtonsLayout);

    // selectionEdit
    selectionEditLayout->addWidget(shiftSelectionLeft,0,0,1,2);
    selectionEditLayout->addWidget(shiftSelectionRight,0,2,1,2);
    selectionEditLayout->addWidget(growSelectionLeft,1,0);
    selectionEditLayout->addWidget(shrinkSelectionLeft,1,1);
    selectionEditLayout->addWidget(shrinkSelectionRight,1,2);
    selectionEditLayout->addWidget(growSelectionRight,1,3);

    selectionEditFrame->setLayout(selectionEditLayout);

    // Buttons
    buttonsLayout->setAlignment(Qt::AlignTop);
    buttonsLayout->addWidget(mediaControlsLabel);
    buttonsLayout->addWidget(mediaButtonsFrame);
    buttonsLayout->addWidget(offsetSpinBoxLabel);
    buttonsLayout->addWidget(offsetSpinBox);
    buttonsLayout->addWidget(classComboBoxLabel);
    buttonsLayout->addWidget(classComboBox);
    buttonsLayout->addWidget(selectionEditFrame);
    buttonsLayout->addWidget(applyClassLabel);
    buttonsLayout->addWidget(exportData);
    //buttonsLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding));

    buttonsFrame->setLayout(buttonsLayout);
    buttonsFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

//    QSplitter *centralWidget = new QSplitter(Qt::Horizontal, this);
//    centralWidget->addWidget(visualFrame);
//    centralWidget->addWidget(buttonsFrame);
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *centralLayout = new QHBoxLayout;
    centralLayout->addWidget(visualFrame);
    centralLayout->addWidget(buttonsFrame);
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);


    connect(_player, &QMediaPlayer::durationChanged, this, [&](qint64) {emit newVideoLoaded(*_player); });
    connect(this, &MainWindow::newVideoLoaded, _t, &Timeline::newVideoLoaded);
    connect(_t->s->position, &SeekerPosition::positionChanged, _player, &QMediaPlayer::setPosition);
    connect(_player, &QMediaPlayer::positionChanged, _t->s->position, &SeekerPosition::setPosition);

    // connect all UI buttons
    connect(stepBack, &QPushButton::clicked, this, &MainWindow::onActionStepBackTriggered);
    connect(playPause, &QPushButton::clicked, this, &MainWindow::onActionPlayPauseTriggered);
    connect(stepForward, &QPushButton::clicked, this, &MainWindow::onActionStepForwardTriggered);
    connect(stop, &QPushButton::clicked, this, &MainWindow::onActionStopTriggered);

    connect(offsetSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::onActionOffsetSpinBoxValueChanged);
    connect(classComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onActionClassComboBoxCurrentTextChanged);

    connect(shiftSelectionLeft, &QPushButton::clicked, this, &MainWindow::onActionShiftSelectionLeftTriggered);
    connect(shiftSelectionRight, &QPushButton::clicked, this, &MainWindow::onActionShiftSelectionRightTriggered);
    connect(growSelectionLeft, &QPushButton::clicked, this, &MainWindow::onActionGrowSelectionLeftTriggered);
    connect(shrinkSelectionLeft, &QPushButton::clicked, this, &MainWindow::onActionShrinkSelectionLeftTriggered);
    connect(growSelectionRight, &QPushButton::clicked, this, &MainWindow::onActionGrowSelectionRightTriggered);
    connect(shrinkSelectionRight, &QPushButton::clicked, this, &MainWindow::onActionShrinkSelectionRightTriggered);

    connect(applyClassLabel, &QPushButton::clicked, this, &MainWindow::onActionApplyClassLabelTriggered);
    connect(exportData, &QPushButton::clicked, this, &MainWindow::onActionExportDataTriggered);

//    setStyleSheet("QWidget {"
//                  "border: 0.5px solid blue;"
//                  "color: red"
//                  "}");

}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _player;
    delete _vw;
    delete _t;
}

void MainWindow::on_actionOpenVideo_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a Video File", "", "Video File (*.avi, *.mpg, *.mp4)");
   // QString filename = "/Users/lukasz/Downloads/Be11-test1 (1).mp4";
    onActionStopTriggered();

    _player->setSource(QUrl::fromLocalFile(filename));
    onActionPlayPauseTriggered();
}


void MainWindow::on_actionOpenCSV_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open a CSV File", "", "CSV File (*.csv)");
    //QString filename = "/Users/Lukasz/Documents/Mosaic/sensor_reading/data/test_data.csv";

    qDebug() << "Opening CSV File";
    TimeSeriesData *tmp = new TimeSeriesData(_t, filename);
    delete _data;
    _data = tmp;
    _t->setDataLoaded(true);
    connect(_t->s, &Seeker::replotNow, _data, &TimeSeriesData::updateData);
    connect(_data, &TimeSeriesData::replotNow, _t, &Timeline::updateTimeline);
    connect(_t, &Timeline::updateWorkspacePlot, _plot, &WorkspacePlot::updateWorkspacePlot);

    qDebug() << "Length of data: " << _data->time.size();
    emit _data->replotNow(_data);
}


void MainWindow::on_actionPlay_triggered()
{
    _player->play();
    _ui->statusbar->showMessage("Playing");
}


void MainWindow::on_actionPause_triggered()
{
    _player->pause();
    _ui->statusbar->showMessage("Paused..");
}


void MainWindow::onActionStopTriggered()
{
    _player->stop();
    _isVideoPlaying = false;
    _ui->statusbar->showMessage("Stopped");
}

void MainWindow::onActionStepBackTriggered()
{
    qint64 currPos = _player->position();
    if (currPos - 1000 > 0) {
        _player->setPosition(currPos-1000);
    }
}

void MainWindow::onActionPlayPauseTriggered()
{
    if (_isVideoPlaying) {
        _player->pause();
        _isVideoPlaying = false;
        _ui->statusbar->showMessage("Paused..");
    } else {
        _player->play();
        _isVideoPlaying = true;
        _ui->statusbar->showMessage("Playing");
    }

}

void MainWindow::onActionStepForwardTriggered()
{
    qint64 currPos = _player->position();
    if (currPos + 1000 < _player->duration()) {
        _player->setPosition(currPos+1000);
    }
}

void MainWindow::onActionOffsetSpinBoxValueChanged(double d)
{
    qDebug() << "Offset Value Changed to :" << d;
    if (_data != nullptr) {
        _data->offset->set(double(d));
    }
}
void MainWindow::onActionClassComboBoxCurrentTextChanged(const QString &text)
{
    qDebug() << "Class Label Changed to :" << text;
}

void MainWindow::onActionShiftSelectionLeftTriggered()
{
    qDebug() << "shiftSelectionLeft triggered";
}

void MainWindow::onActionShiftSelectionRightTriggered()
{
    qDebug() << "shiftSelectionRight triggered";
}

void MainWindow::onActionGrowSelectionLeftTriggered()
{
    qDebug() << "growSelectionLeft triggered";
}

void MainWindow::onActionShrinkSelectionLeftTriggered()
{
    qDebug() << "shrinkSelectionLeft triggered";
}

void MainWindow::onActionGrowSelectionRightTriggered()
{
    qDebug() << "growSelectionRight triggered";
}

void MainWindow::onActionShrinkSelectionRightTriggered()
{
    qDebug() << "shrinkSelectionRight triggered";
}

void MainWindow::onActionApplyClassLabelTriggered()
{
    qDebug() << "applyClassLabel triggered";
}

void MainWindow::onActionExportDataTriggered()
{
    qDebug() << "exportData triggered";
}

void MainWindow::on_actionEdit_Class_Labels_triggered()
{
    qDebug() << "Editing Available Class Labels";
}

