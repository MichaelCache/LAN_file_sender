#include "recieve_progress_model.h"

#include <QMutexLocker>
#include <stdexcept>

using TransferProgress::Column;

RecieveProgressModel::RecieveProgressModel(QObject *parent)
    : ProgressModel(parent) {}

RecieveProgressModel::~RecieveProgressModel() {}

QVariant RecieveProgressModel::headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    Column col = (Column)section;
    switch (col) {
      case Column::IP:
        return "FromIP";
      case Column::FileName:
        return "FileName";
      case Column::FileSize:
        return "Size";
      case Column::State:
        return "State";
      case Column::Progress:
        return "Progress";
      default:
        return QVariant();
    }
  }

  return QVariant();
}
