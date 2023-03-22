#include "receiver_model.h"

#include <QHostAddress>

#include "column.h"


using RemoteClient::Column;

ReceiverModel::ReceiverModel(QObject *parent) : QAbstractTableModel(parent) {}

ReceiverModel::~ReceiverModel() {}

bool ReceiverModel::contains(QHostAddress addr) {
  return m_remote_servers_addrs.contains(addr.toString());
}

bool ReceiverModel::add(const RemoteServer &server) {
  if (contains(server.m_host_addr)) {
    int row = 0;
    auto find = std::find_if(m_remote_servers.begin(), m_remote_servers.end(),
                             [&server, &row](const RemoteServer &s) {
                               row++;
                               return s.m_host_addr == server.m_host_addr;
                             });
    if (find != m_remote_servers.end()) {
      if (*find == server) {
        return false;
      } else {
        find->m_host_name = server.m_host_name;
        find->m_os = server.m_os;
        qDebug() << "update server: " << server.m_host_addr;
        emit dataChanged(index(row, 0), index(row, (int)Column::Count));
        return true;
      }
    } else {
      return false;
    }

  } else {
    qDebug() << "add new server: " << server.m_host_addr;
    m_remote_servers.push_back(server);
    m_remote_servers_addrs.insert(server.m_host_addr.toString());
    emit layoutChanged();
    return true;
  }
}

int ReceiverModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return m_remote_servers.size();
}

int ReceiverModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return (int)Column::Count;
}

QVariant ReceiverModel::data(const QModelIndex &index, int role) const {
  if (index.isValid()) {
    Column col = (Column)index.column();

    auto receiver = m_remote_servers.at(index.row());

    if (role == Qt::DisplayRole) {
      switch (col) {
        case Column::IP:
          return receiver.m_host_addr.toString();
        case Column::Name:
          return receiver.m_host_name;
        case Column::OS:
          return receiver.m_os;
        default:
          return QVariant();
      }
    }

    return QVariant();
  }
  return QVariant();
}

QVariant ReceiverModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    Column col = (Column)section;
    switch (col) {
      case Column::IP:
        return "Ip";
      case Column::Name:
        return "Name";
      case Column::OS:
        return "OS";
      default:
        return QVariant();
    }
  }

  return QVariant();
}