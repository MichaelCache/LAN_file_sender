#include "recieve_progress_model.h"

#include <QMutexLocker>
#include <stdexcept>

using TransferProgress::Column;

RecieveProgressModel::RecieveProgressModel(QObject *parent)
    : ProgressModel(parent) {}

RecieveProgressModel::~RecieveProgressModel() {}

QVariant RecieveProgressModel::data(const QModelIndex &index, int role) const {
  if (index.isValid()) {
    Column col = (Column)index.column();

    auto task = m_tasks.at(index.row());
    if (role == Qt::DisplayRole) {
      if (col == Column::IP) {
        return task.m_from_ip.toString();
      } else {
        return ProgressModel::data(index, role);
      }
    }
    return ProgressModel::data(index, role);
  }
  return ProgressModel::data(index, role);
}

QVariant RecieveProgressModel::headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    Column col = (Column)section;
    if (col == Column::IP) {
      return "FromIP";
    } else {
      return ProgressModel::headerData(section, orientation, role);
    }
  }
  return QVariant();
}
