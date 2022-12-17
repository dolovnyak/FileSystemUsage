#ifndef USAGEMODEL_H
#define USAGEMODEL_H

#include <QAbstractTableModel>

#include "usagegatherer.h"

class UsageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit UsageModel();

    ~UsageModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void selectDirectory(const QString &directoryPath);

public slots:
    void updateSelectedDirectory(const QString &directoryPath, const QVector<ModelView> &directoryUsage);

private:
    QString _selectedDirectoryPath = nullptr;

    QVector<ModelView> _selectedDirectoryUsage = {{"All", "--", "--", "--", "--"}};
};

#endif // USAGEMODEL_H
