#ifndef USAGEMODEL_H
#define USAGEMODEL_H

#include <QAbstractTableModel>

#include "usagegatherer.h"

class UsageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit UsageModel(QFileSystemModel *fileSystemModel);

    ~UsageModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

signals:
    void directorySelected(QString path);

public slots:
    void selectDirectory(const QModelIndex &index);

    void updateSelectedDirectory(const QString& directoryPath, const QVector<ModelView> &directoryUsage);

private:
    QFileSystemModel *_fileSystemModel;

    QString _selectedDirectoryPath = nullptr;

    QVector<ModelView> _selectedDirectoryUsage = {{"All", "--", "--", "--", "--"}};
};

#endif // USAGEMODEL_H
