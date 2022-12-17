#include "gathererstatusmodel.h"

GathererStatusModel::GathererStatusModel()
{

}

int GathererStatusModel::rowCount(const QModelIndex &) const
{
    return _statuses.count();
}

QVariant GathererStatusModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return _statuses[index.row()];
    }
    return QVariant();
}

void GathererStatusModel::UpdateStatus(const QVector<QString> &statuses)
{
    this->beginResetModel();
    _statuses = statuses;
    this->endResetModel();
}
