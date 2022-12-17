#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "usagemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void driveSelected(const QModelIndex &index);

    void directorySelected(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    QFileSystemModel *_driveModel;
    QFileSystemModel *_fileSystemModel;
    UsageModel *_usageModel;
    UsageGatherer *_usageGatherer;
    GathererStatusModel *_gathererStatusModel;
};
#endif // MAINWINDOW_H
