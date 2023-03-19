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
  return m_remote_servers.contains(addr.toString());
}

void ReceiverModel::add(RemoteServer *server) {
  m_remote_servers.insert(server->addr().toString(), server);
}

int ReceiverModel::rowCount(const QModelIndex &parent) const { return 1; }
int ReceiverModel::columnCount(const QModelIndex &parent) const { return 3; }
QVariant ReceiverModel::data(const QModelIndex &index, int role) const {
  if (index.isValid()) {
    Column col = (Column)index.column();

    if (role == Qt::DisplayRole) {
      switch (col) {
        case Column::IP:
          return "192.168.1.1";
        case Column::Name:
          return "LocalHost";
        case Column::OS:
          return "Win";
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