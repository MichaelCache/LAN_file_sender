#include "receiver_model.h"

#include <QHostAddress>

#include "column.h"

using RemoteClient::Column;

ReceiverModel::ReceiverModel(QObject *parent) : QAbstractTableModel(parent) {}

ReceiverModel::~ReceiverModel() {}

bool ReceiverModel::contains(QHostAddress addr) {
  return m_remote_servers_addrs.contains(addr.toString());
}

void ReceiverModel::add(const RemoteHostInfo &server) {
  if (contains(server.m_host_addr)) {
    int row = 0;
    // find remote server in same ip, name or os may changed
    auto find = std::find_if(m_remote_servers.begin(), m_remote_servers.end(),
                             [&server, &row](const RemoteHostInfo &s) {
                               row++;
                               return s.m_host_addr == server.m_host_addr;
                             });
    if (find != m_remote_servers.end()) {
      if (*find == server) {
        // remote server info not changed, no need update
        return;
      } else {
        find->m_host_name = server.m_host_name;
        find->m_os = server.m_os;
        qDebug() << "update server: " << server.m_host_addr;
        emit dataChanged(index(row, 0), index(row, (int)Column::Count));
        return;
      }
    } else {
      return;
    }

  } else {
    qDebug() << "add new server: " << server.m_host_addr;
    m_remote_servers.push_back(server);
    m_remote_servers_addrs.insert(server.m_host_addr.toString());
    emit layoutChanged();
    return;
  }
}

void ReceiverModel::remove(const RemoteHostInfo &server) {
  auto it = m_remote_servers_addrs.find(server.m_host_addr.toString());
  if (it != m_remote_servers_addrs.end()) {
    m_remote_servers_addrs.erase(it);
  }

  auto find = std::find_if(m_remote_servers.begin(), m_remote_servers.end(),
                           [&server](const RemoteHostInfo &s) {
                             return s.m_host_addr == server.m_host_addr;
                           });
  if (find != m_remote_servers.end()) {
    qDebug() << "remove server: " << server.m_host_addr;
    m_remote_servers.erase(find);
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