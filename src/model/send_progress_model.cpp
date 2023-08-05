#include "send_progress_model.h"

#include <QMutexLocker>
#include <stdexcept>

using TransferProgress::Column;

namespace {
QString sizeToString(qint64 size) {
  int count = 0;
  double f_size = size;
  while (f_size >= 1024) {
    f_size /= 1024;
    count++;
  }

  QString suffix;
  switch (count) {
    case 0:
      suffix = " B";
      break;
    case 1:
      suffix = " KB";
      break;
    case 2:
      suffix = " MB";
      break;
    case 3:
      suffix = " GB";
      break;
    case 4:
      suffix = " TB";
      break;
  }

  return QString::number(f_size, 'f', 2).append(suffix);
}

QString stateToString(TransferState state) {
  switch (state) {
    case TransferState::Waiting:
      return "Waiting";
    case TransferState::Disconnected:
      return "Disconnected";
    case TransferState::Paused:
      return "Paused";
    case TransferState::Cancelled:
      return "Cancelled";
    case TransferState::Transfering:
      return "Transfering";
    case TransferState::Finish:
      return "Finish";
    case TransferState::UnKonwn:
      return "UnKonwn";
  }

  return QString();
}

}  // namespace

SendProgressModel::SendProgressModel(QObject *parent) : ProgressModel(parent) {}

SendProgressModel::~SendProgressModel() {}

QVariant SendProgressModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    Column col = (Column)section;
    switch (col) {
      case Column::IP:
        return "DestIP";
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
