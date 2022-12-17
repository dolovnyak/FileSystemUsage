#include "usagegatherer.h"
#include "defines.h"

namespace {

QVector<ModelView> toModelView(qint64 subdirsQuantity, const std::unordered_map<QString, FilesUsage> &map) {
    QVector<ModelView> res;

    res.push_back({"All", 0, 0, 0, 0});

    qint64 allFilesQuantity = 0;
    qint64 allFilesTotalSize = 0;

    for(auto it = map.begin(); it != map.end(); ++it) {
        ModelView view;

        allFilesQuantity += it->second.filesQuantity;
        allFilesTotalSize += it->second.filesTotalSize;

        view.subdirsQuantity = "--";
        view.filesQuantity = QString::number(it->second.filesQuantity);
        view.filesTotalSize = QString::number(it->second.filesTotalSize) + " B";
        view.filesMediumSize = QString::number(it->second.filesTotalSize / it->second.filesQuantity) + " B";
        view.type = it->first;

        res.push_back(view);
    }

    res[0].filesQuantity = QString::number(allFilesQuantity);
    res[0].filesTotalSize = QString::number(allFilesTotalSize) + " B";
    res[0].filesMediumSize = QString::number(allFilesQuantity == 0 ? 0 : allFilesTotalSize / allFilesQuantity) + " B";
    res[0].subdirsQuantity = QString::number(subdirsQuantity);

    return res;
}

std::unordered_map<QString, FilesUsage> addMaps(std::unordered_map<QString, FilesUsage> map1, std::unordered_map<QString, FilesUsage> map2)
{
    for (auto it1 = map1.begin(); it1 != map1.end(); ++it1) {
        auto it2 = map2.find(it1->first);
        if (it2 != map2.end()) {
            it1->second = it1->second + it2->second;
        }
    }

    for (auto it2 = map2.begin(); it2 != map2.end(); ++it2) {
        auto it1 = map1.find(it2->first);
        if (it1 == map1.end()) {
            map1.emplace(it2->first, it2->second);
        }
    }

    return map1;
}

}

FilesUsage &FilesUsage::operator+(const FilesUsage &other)
{
    filesTotalSize += other.filesTotalSize;
    filesQuantity += other.filesQuantity;

    return *this;
}

UsageGatherer::UsageGatherer()
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();
}

UsageGatherer::~UsageGatherer()
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();
}

void UsageGatherer::run()
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    try
    {
        QElapsedTimer clearObsoleteNodesTimer;
        clearObsoleteNodesTimer.start();

        while (!isInterruptionRequested()) {
            _rootMutex.lock();
            QString root = _root;
            _rootMutex.unlock();

            if (root.isEmpty()) {
                QThread::msleep(5); /// sleep until drive selected
                continue;
            }

            _rootChanged.storeRelease(0);

            calculateDirectoryUsage(root);

            if (clearObsoleteNodesTimer.hasExpired(CLEAR_OBSOLETE_NODES_TIMEOUT_MS)) {
                clearObsoleteNodes();
                clearObsoleteNodesTimer.restart();
            }
        }
    }
    catch (const std::exception &e) {
        qCritical() << e.what();
    }
}

void UsageGatherer::selectDirectory(const QString &directoryPath)
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    _selectedDirectoryMutex.lock();
    _selectedDirectoryPath = directoryPath;
    _selectedDirectoryMutex.unlock();

    _directoryChanged.storeRelease(1);
}

void UsageGatherer::setRootPath(const QString &rootPath)
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    _rootMutex.lock();
    _root = rootPath;
    _rootMutex.unlock();

    _rootChanged.storeRelease(1);
}

std::unordered_map<QString, FilesUsage> UsageGatherer::calculateDirectoryUsage(QDir currentDir)
{
    if (isInterruptionRequested() || _rootChanged.loadAcquire()) {
        return {};
    }
    std::unordered_map<QString, FilesUsage> res;
    qint64 subdirsQuantity = 0;
    QFileInfoList filesList = currentDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::NoSymLinks, QDir::NoSort);
    QString currentDirPath = currentDir.absolutePath();

    for (auto &elem : filesList) {
        if (elem.isFile()) {
            QString extension = "." + elem.suffix();

            auto it = res.find(extension);
            if (it != res.end()) {
                it->second.filesQuantity += 1;
                it->second.filesTotalSize += elem.size();
            }
            else {
                res.emplace(std::move(extension), FilesUsage{1, elem.size()});
            }
        }
        else {
            ++subdirsQuantity;
            res = addMaps(std::move(res), calculateDirectoryUsage(elem.absoluteFilePath()));

        }
    }

    QElapsedTimer lifeTimer;
    lifeTimer.start();
    _usageByDir[currentDirPath] = DirectoryUsage{subdirsQuantity, res, std::move(lifeTimer)};

    _selectedDirectoryMutex.lock();
    auto selectedDirectory = _selectedDirectoryPath;
    _selectedDirectoryMutex.unlock();

    if (selectedDirectory == currentDirPath) {
        emit selectedDirectoryChanged(currentDirPath, toModelView(subdirsQuantity, res));
    }
    else if (_directoryChanged.testAndSetOrdered(1, 0)) {
        auto it = _usageByDir.find(selectedDirectory);
        if (it != _usageByDir.end()) {
            emit selectedDirectoryChanged(selectedDirectory, toModelView(it->second.subdirsNum, it->second.usageByExtension));
        }
    }

    emit statusInfo({currentDirPath});

    return res;
}

void UsageGatherer::clearObsoleteNodes()
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    for (auto it = _usageByDir.begin(); it != _usageByDir.end();) {
        if (it->second.lifeTimer.hasExpired(NODE_LIFETIME_MS)) {
            it = _usageByDir.erase(it);
        }
        else {
            ++it;
        }
    }
}
