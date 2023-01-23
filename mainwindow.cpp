#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QBoxLayout>
#include <QFrame>
#include <QSplitter>
#include <QSpacerItem>
#include <QFile>
#include <QByteArray>
#include <QJsonDocument>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindow)
    , _offsetSpinBox(new QDoubleSpinBox(this))
    , _classComboBox(new QComboBox(this))
    , _player(new QMediaPlayer(this))
    , _vw(new QVideoWidget(this))
    , _t(new Timeline(this))
    , _plot(new WorkspacePlot(_t))
    , _data(new TimeSeriesData(_t))
    , _lbl(new ClassLabels())
    , _isVideoPlaying(false)
    , stepBack(new QPushButton(this))
    , playPause(new QPushButton(this))
    , stepForward(new QPushButton(this))
    , stop(new QPushButton(this))
{
    _ui->setupUi(this);

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

    // Media Buttons Init
    QFrame *mediaButtonsFrame = new QFrame(this);
    // step back
    QIcon backIcon(":/icons/StepBack.png");
    stepBack->setIcon(backIcon);
    stepBack->setIconSize(QSize(32,32));
    // play pause
    QIcon playIcon(":/icons/Play.png");
    playPause->setIcon(playIcon);
    playPause->setIconSize(QSize(32,32));
    // step forward
    QIcon forwardIcon(":/icons/StepForward.png");
    stepForward->setIcon(forwardIcon);
    stepForward->setIconSize(QSize(32,32));
    // stop
    QIcon stopIcon(":/icons/Stop.png");
    stop->setIcon(stopIcon);
    stop->setIconSize(QSize(32,32));


    QLabel *offsetSpinBoxLabel = new QLabel("Data Position", this);
    offsetSpinBoxLabel->setAlignment(Qt::AlignCenter);
    _offsetSpinBox->setRange(-1.79769E+308, 1.79769E+308);
    _offsetSpinBox->setDecimals(4);

    QLabel *classComboBoxLabel = new QLabel("Class Label", this);
    classComboBoxLabel->setAlignment(Qt::AlignCenter);

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
    buttonsLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    buttonsLayout->addWidget(mediaControlsLabel);
    buttonsLayout->addWidget(mediaButtonsFrame);
    buttonsLayout->addWidget(offsetSpinBoxLabel);
    buttonsLayout->addWidget(_offsetSpinBox);
    buttonsLayout->addWidget(classComboBoxLabel);
    buttonsLayout->addWidget(_classComboBox);
    buttonsLayout->addWidget(selectionEditFrame);
    buttonsLayout->addWidget(applyClassLabel);
    buttonsLayout->addSpacerItem(new QSpacerItem(10, 1000, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QFrame *line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(320, 150, 118, 3));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    buttonsLayout->addWidget(line);
    buttonsLayout->addWidget(exportData);
    buttonsLayout->setAlignment(exportData, Qt::AlignBottom);

    //buttonsLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding));

    buttonsFrame->setLayout(buttonsLayout);
    buttonsFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *centralLayout = new QHBoxLayout;
    centralLayout->addWidget(visualFrame);
    centralLayout->addWidget(buttonsFrame);
    centralLayout->setAlignment(buttonsFrame, Qt::AlignTop);
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);


    connect(_player, &QMediaPlayer::durationChanged, this, [&](qint64) {emit newVideoLoaded(*_player); });
    connect(this, &MainWindow::newVideoLoaded, _t, &Timeline::newVideoLoaded);
    connect(_t->s->position, &SeekerPosition::positionChanged, _player, &QMediaPlayer::setPosition);
    connect(_player, &QMediaPlayer::positionChanged, _t->s->position, &SeekerPosition::setPosition);
    connect(this, &MainWindow::isVideoPlaying, _t, &Timeline::isVideoPlaying);

    // connect all UI buttons
    connect(stepBack, &QPushButton::clicked, this, &MainWindow::onActionStepBackTriggered);
    connect(playPause, &QPushButton::clicked, this, &MainWindow::onActionPlayPauseTriggered);
    connect(stepForward, &QPushButton::clicked, this, &MainWindow::onActionStepForwardTriggered);
    connect(stop, &QPushButton::clicked, this, &MainWindow::onActionStopTriggered);

    connect(_offsetSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::onActionOffsetSpinBoxValueChanged);
    connect(_classComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onActionClassComboBoxCurrentTextChanged);

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
//    QString filename = QFileDialog::getOpenFileName(this, "Open a Video File", "", "Video File (*.avi, *.mpg, *.mp4, *.mov)");
    QString filename = "/Users/lukasz/Documents/Mosaic/data_labelling/VideoRecording/first_layer_413_12fps.mov";
    onActionStopTriggered();

    _player->setSource(QUrl::fromLocalFile(filename));
    onActionPlayPauseTriggered();
}


void MainWindow::on_actionOpenCSV_triggered()
{
//    QString filename = QFileDialog::getOpenFileName(this, "Open a CSV File", "", "CSV File (*.csv)");
    QString filename = "/Users/lukasz/Documents/Mosaic/data_labelling/labelled_first_layer_413.csv";

    qDebug() << "Opening CSV File";
    TimeSeriesData *tmp = new TimeSeriesData(_t, filename);
    delete _data;
    _data = tmp;
    _t->setDataLoaded(true);
    connect(_t->s, &Seeker::replotNow, _data, &TimeSeriesData::updateData);
    connect(_data, &TimeSeriesData::replotNow, _t, &Timeline::updateTimeline);
    connect(_t, &Timeline::updateWorkspacePlot, _plot, &WorkspacePlot::updateWorkspacePlot);
    connect(_data->offset, &SeekerPosition::positionChanged, this, [&](float pos){
        _offsetSpinBox->setValue(pos);
    });
    connect(_plot, &WorkspacePlot::workspaceSelection, _data, &TimeSeriesData::workspaceSelection);
    connect(this, &MainWindow::applyClassLabel, _data, &TimeSeriesData::applyClassLabel);
    connect(this, &MainWindow::exportData, _data, &TimeSeriesData::exportData);
    connect(_data, &TimeSeriesData::onDoneCSVExport, this, &MainWindow::onDoneCSVExport);

    qDebug() << "Length of data: " << _data->time.size();
    emit _data->replotNow(_data);
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
        QString msg = "Paused @ " + QString::number(_player->position());
        _ui->statusbar->showMessage(msg);
        QIcon playIcon(":/icons/Play.png");
        playPause->setIcon(playIcon);
    } else {
        _player->play();
        _isVideoPlaying = true;
        _ui->statusbar->showMessage("Playing");
        QIcon playIcon(":/icons/Pause.png");
        playPause->setIcon(playIcon);
    }
    emit isVideoPlaying(_isVideoPlaying);
    emit _data->replotNow(_data);
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
        emit _data->replotNow(_data);
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
    qDebug() << "applyClassLabel triggered. Index: " << _classComboBox->currentIndex();
    if (_classComboBox->count() != 0) {
        emit applyClassLabel(_classComboBox->currentIndex());
    }
}

void MainWindow::onActionExportDataTriggered()
{
    qDebug() << "exportData triggered";
    if (_data != nullptr)  {
        _ui->statusbar->showMessage("Exporting data to CSV");
        QString filename = QFileDialog::getSaveFileName(this, "Save File", "", "CSV File (*.csv)");
        emit exportData(filename);
    }
}

void MainWindow::on_actionEdit_Class_Labels_triggered()
{
    qDebug() << "Editing Available Class Labels";
    QString filename = QFileDialog::getOpenFileName(this, "Open a JSON File", "", "JSON File (*.json)");
    QFile inFile(filename);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();
    QJsonParseError errorPtr;
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &errorPtr);
    if (jsonDoc.isNull()) {
        qDebug() << "Error opening JSON file: " << errorPtr.errorString();
        return;
    }

    QJsonObject rootObj = jsonDoc.object();
    QJsonArray lblArray = rootObj.value("labels").toArray();
    for (const auto e : lblArray) {
        qDebug() << "looping thorugh this";
        for (const auto &key : e.toObject().keys()) {
            qDebug() << key;
            _data->lbls->insert(e.toObject().value(key).toInt(), key);
            _classComboBox->insertItem(e.toObject().value(key).toInt(), key);
        }
    }

}

void MainWindow::onDoneCSVExport(const QString& filename)
{
    QString msg("Done Exporting CSV to : " + filename);
    _ui->statusbar->showMessage(msg);
}

