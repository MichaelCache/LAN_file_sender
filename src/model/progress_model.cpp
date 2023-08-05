#include "progress_model.h"

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

ProgressModel::ProgressModel(QObject *parent) : ProgressInterface(parent) {}

ProgressModel::~ProgressModel() {}

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
        case Column::IP:
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
    } else if (role == MyRole::InfoRole) {
      return QVariant().fromValue(task);
    } else {
      return QVariant();
    }
  } else {
    return QVariant();
  }
}

void ProgressModel::add(QVector<TransferInfo> info) {
  QMutexLocker locker(&m_lock);
  emit layoutAboutToBeChanged();
  for (auto &&i : info) {
    m_tasks.append(i);
  }
  emit layoutChanged();
}

void ProgressModel::remove(QVector<TransferInfo> info) {
  QMutexLocker locker(&m_lock);
  emit layoutAboutToBeChanged();
  for (auto &&i : info) {
    m_tasks.removeAll(i);
  }
  emit layoutChanged();
}

void ProgressModel::update(QVector<TransferInfo> info) {
  QMutexLocker locker(&m_lock);
  for (auto &&task : info) {
    auto find = std::find(m_tasks.begin(), m_tasks.end(), task);
    if (find != m_tasks.end()) {
      *find = task;
      int row = m_tasks.indexOf(*find);
      emit dataChanged(index(row, (int)Column::State),
                       index(row, (int)Column::Progress));
    }
  }
}

const TransferInfo &ProgressModel::get(QUuid id) {
  QMutexLocker locker(&m_lock);
  auto it =
      std::find_if(m_tasks.begin(), m_tasks.end(),
                   [&id](const TransferInfo &info) { return id == info.id(); });

  if (it != m_tasks.end()) {
    return *it;
  } else {
    throw std::runtime_error("No transfer info found");
  }
}

void ProgressModel::clear() {
  QMutexLocker locker(&m_lock);
  emit layoutAboutToBeChanged();
  QVector<TransferInfo> marked_remove;
  for (auto &&i : m_tasks) {
    if (i.m_state == TransferState::Cancelled ||
        i.m_state == TransferState::Disconnected ||
        i.m_state == TransferState::Finish) {
      marked_remove.push_back(i);
    }
  }
  for (auto &i : marked_remove) {
    m_tasks.removeAll(i);
  }
  emit layoutChanged();
}
