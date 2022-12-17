#ifndef GATHERERSTATUSMODEL_H
#define GATHERERSTATUSMODEL_H

#include <QAbstractListModel>

class GathererStatusModel : public QAbstractListModel
{
    Q_OBJECT
public:
    GathererStatusModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void UpdateStatus(const QVector<QString> &statuses);

private:
    QVector<QString> _statuses;
};

#endif // GATHERERSTATUSMODEL_H
