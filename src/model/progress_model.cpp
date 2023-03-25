#include "progress_model.h"

#include "column.h"


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
    case TransferState::Idle:
      return "Idle";
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
  }

  return QString();
}

}  // namespace

ProgressModel::ProgressModel(QObject *parent) : QAbstractTableModel(parent) {}

ProgressModel::~ProgressModel() {}

void ProgressModel::add(const TransferInfo &task) {
  m_tasks.push_back(task);
  emit layoutChanged();
}

int ProgressModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return m_tasks.size();
}
int ProgressModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return (int)Column::Count;
}

QVariant ProgressModel::data(const QModelIndex &index, int role) const {
  if (index.isValid()) {
    Column col = (Column)index.column();

    auto task = m_tasks.at(index.row());

    if (role == Qt::DisplayRole) {
      switch (col) {
        case Column::DestIP:
          return task.m_dest_ip.toString();
        case Column::FileName:
          return task.m_file_name;
        case Column::FileSize:
          return sizeToString(task.m_file_size);
        case Column::State:
          return stateToString(task.m_state);
        case Column::Progress:
          return task.m_progress;
        default:
          return QVariant();
      }
    }

    return QVariant();
  }
  return QVariant();
}

QVariant ProgressModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    Column col = (Column)section;
    switch (col) {
      case Column::DestIP:
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
