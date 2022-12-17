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
    ui->setupUi(this);

    QStorageInfo storage = QStorageInfo::root();

    _dirModel = new QFileSystemModel;
    _dirModel->setReadOnly(true);
    _dirModel->setRootPath(storage.rootPath());
    _dirModel->setOption(QFileSystemModel::DontUseCustomDirectoryIcons);
    _dirModel->setFilter(QDir::Drives | QDir::NoDotAndDotDot | QDir::Dirs | QDir::NoSymLinks);

    ui->treeView->setModel(_dirModel);
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
    ui->treeView->setSortingEnabled(false);

    _usageModel = new UsageModel(_dirModel);
    QObject::connect(ui->treeView, &QAbstractItemView::pressed, _usageModel, &UsageModel::selectDirectory);

    _usageGatherer = new UsageGatherer(storage.rootPath());
    QObject::connect(_usageModel, &UsageModel::directorySelected, _usageGatherer, &UsageGatherer::selectDirectory);
    QObject::connect(_usageGatherer, &UsageGatherer::selectedDirectoryChanged, _usageModel, &UsageModel::updateSelectedDirectory);
    _usageGatherer->start();

    _gathererStatusModel = new GathererStatusModel;
    QObject::connect(_usageGatherer, &UsageGatherer::statusInfo, _gathererStatusModel, &GathererStatusModel::UpdateStatus);

    ui->tableView->setModel(_usageModel);
    ui->listView->setModel(_gathererStatusModel);

    ui->treeView->show();
    ui->tableView->show();
    ui->listView->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _usageModel;
    delete _dirModel;

    _usageGatherer->requestInterruption();
    _usageGatherer->wait();
    delete _usageGatherer;
}


