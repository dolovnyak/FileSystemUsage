#ifndef USAGEGATHERER_H
#define USAGEGATHERER_H

#include <QThread>
#include <QFileSystemModel>
#include <QMutex>

#include <unordered_map>

#include "gathererstatusmodel.h"

class ModelView {
public:
    QString type;

    QString filesQuantity;
    QString filesTotalSize;
    QString filesMediumSize;
    QString subdirsQuantity;
};

class FilesUsage {
public:
    FilesUsage& operator+(const FilesUsage &other);

    qint64 filesQuantity;
    qint64 filesTotalSize;
};

class DirectoryUsage {
public:
    qint64 subdirsNum = 0;
    std::unordered_map<QString, FilesUsage> usageByExtension;
    QElapsedTimer lifeTimer;
};

class UsageGatherer : public QThread
{
    Q_OBJECT
public:
    UsageGatherer(const QString &root);

    ~UsageGatherer();

    void run() override;

signals:
    void selectedDirectoryChanged(QString directoryPath, QVector<ModelView> directoryUsage);

    void statusInfo(QVector<QString> statuses);

public slots:
    void selectDirectory(const QString &directoryPath);

    void changeRoot(const QString &rootPath);

private:
    std::unordered_map<QString, FilesUsage> calculateDirectoryUsage(QDir currentDir);

    void clearObsoleteNodes();

    std::unordered_map<QString, DirectoryUsage> _usageByDir;

    QString _root;
    QMutex _rootMutex;

    QString _selectedDirectoryPath;
    QMutex _selectedDirectoryMutex;

    QAtomicInt _directoryChanged;
};

#endif // USAGEGATHERER_H
