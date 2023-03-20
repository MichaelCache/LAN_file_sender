#include "receiver_model.h"

#include <QHostAddress>

#include "column.h"

ReceiverModel::ReceiverModel(QObject *parent) : QAbstractTableModel(parent) {
  // setColumnCount(3);
  // setHeaderData(0, Qt::Horizontal, "IP");
  // setHeaderData(1, Qt::Horizontal, "Name");
  // setHeaderData(2, Qt::Horizontal, "OS");
}

ReceiverModel::~ReceiverModel() {}

bool ReceiverModel::contains(QHostAddress addr) {
  return m_remote_servers_addrs.contains(addr.toString());
}

bool ReceiverModel::add(RemoteServer *server) {
  if (contains(server->m_host_addr)) {
    auto find = std::find_if(m_remote_servers.begin(), m_remote_servers.end(),
                             [&server](const RemoteServer *s) {
                               return s->m_host_addr == server->m_host_addr;
                             });
    if (find != m_remote_servers.end()) {
      if (*(*find) == *server) {
        return false;
      } else {
        (*find)->m_host_name = server->m_host_name;
        (*find)->m_os = server->m_os;
        qDebug() << "update server: " << server->m_host_addr;
        return true;
      }
    } else {
      return false;
    }

  } else {
    qDebug() << "add new server: " << server->m_host_addr;
    m_remote_servers.push_back(server);
    m_remote_servers_addrs.insert(server->m_host_addr.toString());
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
          return receiver->m_host_addr.toString();
        case Column::Name:
          return receiver->m_host_name;
        case Column::OS:
          return receiver->m_os;
        default:
          break;
      }
    }

    return QVariant();
  }
}

QVariant ReceiverModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    Column col = (Column)section;
    switch (col) {
      case Column::IP:
        return tr("Ip");
      case Column::Name:
        return tr("Name");
      case Column::OS:
        return tr("OS");
      default:
        break;
    }
  }

  return QVariant();
}