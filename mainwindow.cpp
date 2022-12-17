#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "usagemodel.h"

#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QStorageInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    /// init
    ui->setupUi(this);
    _driveModel = new QFileSystemModel;
    _fileSystemModel = new QFileSystemModel;
    _usageModel = new UsageModel;
    _usageGatherer = new UsageGatherer;
    _gathererStatusModel = new GathererStatusModel;


    /// setup drives list
    _driveModel->setOption(QFileSystemModel::DontUseCustomDirectoryIcons);
    _driveModel->setFilter(QDir::Drives);
    _driveModel->setRootPath(QStorageInfo::root().rootPath());
    ui->drivesView->setModel(_driveModel);
    QObject::connect(ui->drivesView, &QAbstractItemView::pressed, this, &MainWindow::driveSelected);


    /// setup directories tree
    _fileSystemModel->setReadOnly(true);
    _fileSystemModel->setOption(QFileSystemModel::DontUseCustomDirectoryIcons);
    _fileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::NoSymLinks);
    ui->fileSystemView->setModel(_fileSystemModel);
    ui->fileSystemView->hideColumn(1);
    ui->fileSystemView->hideColumn(2);
    ui->fileSystemView->hideColumn(3);
    ui->fileSystemView->setSortingEnabled(false);
    QObject::connect(ui->fileSystemView, &QAbstractItemView::pressed, this, &MainWindow::directorySelected);


    /// setup usage model, usage gatherer and usage gatherer status model
    ui->usageView->setModel(_usageModel);
    ui->statusView->setModel(_gathererStatusModel);
    QObject::connect(_usageGatherer, &UsageGatherer::selectedDirectoryChanged, _usageModel, &UsageModel::updateSelectedDirectory);
    QObject::connect(_usageGatherer, &UsageGatherer::statusInfo, _gathererStatusModel, &GathererStatusModel::UpdateStatus);

    _usageGatherer->start();

    ui->fileSystemView->hide(); /// hide until drive selected

    ui->drivesView->show();
    ui->usageView->show();
    ui->statusView->show();
}

MainWindow::~MainWindow()
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    _usageGatherer->requestInterruption();
    _usageGatherer->wait();
    delete _usageGatherer;

    delete ui;
    delete _driveModel;
    delete _fileSystemModel;
    delete _usageModel;
    delete _gathererStatusModel;
}

void MainWindow::driveSelected(const QModelIndex &index)
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    QString rootPath = _driveModel->filePath(index);
    _usageModel->selectDirectory(rootPath);
    _usageGatherer->selectDirectory(rootPath);

    if (rootPath == _currentDrive) {
        return;
    }
    _currentDrive = rootPath;
    _fileSystemModel->setRootPath(rootPath);
    _usageGatherer->setRootPath(rootPath);

    ui->fileSystemView->setRootIndex(_fileSystemModel->index(rootPath));
    ui->fileSystemView->show();
}

void MainWindow::directorySelected(const QModelIndex &index)
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    QString directoryPath = _fileSystemModel->filePath(index);
    _usageModel->selectDirectory(directoryPath);
    _usageGatherer->selectDirectory(directoryPath);
}


