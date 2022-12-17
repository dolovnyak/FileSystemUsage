#include "usagemodel.h"

UsageModel::UsageModel()
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();
}

UsageModel::~UsageModel()
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();
}

//-------------------------------------------------------
int UsageModel::rowCount(const QModelIndex & /*parent*/) const
{
    return 4;
}

//-------------------------------------------------------
int UsageModel::columnCount(const QModelIndex & /*parent*/) const
{
    return _selectedDirectoryUsage.size();
}

//-------------------------------------------------------
QVariant UsageModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.row()) {
        case 0:
            return _selectedDirectoryUsage[index.column()].filesTotalSize;
        case 1:
            return _selectedDirectoryUsage[index.column()].filesMediumSize;
        case 2:
            return _selectedDirectoryUsage[index.column()].filesQuantity;
        case 3:
            return _selectedDirectoryUsage[index.column()].subdirsQuantity;
        }
    }
    return QVariant();
}

//! [quoting mymodel_c]
QVariant UsageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (_selectedDirectoryUsage[section].type == ".") {
            return "No extension";
        }
        return _selectedDirectoryUsage[section].type;
    }
    else if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
        switch (section) {
        case 0:
            return QString("Files total size");
        case 1:
            return QString("Files medium size");
        case 2:
            return QString("Files quantity");
        case 3:
            return QString("Subdirs quantity");
        }
    }
    return QVariant();
}

void UsageModel::selectDirectory(const QString &directoryPath)
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    _selectedDirectoryPath = directoryPath;
    this->beginResetModel();
    _selectedDirectoryUsage = {{"All", "--", "--", "--", "--"}};
    this->endResetModel();
}

void UsageModel::updateSelectedDirectory(const QString &directoryPath, const QVector<ModelView> &directoryUsage)
{
    qInfo() << this << Q_FUNC_INFO << QThread::currentThreadId();

    if (directoryPath != _selectedDirectoryPath) {
        return;
    }

    this->beginResetModel();
    _selectedDirectoryUsage = directoryUsage;
    this->endResetModel();
}

