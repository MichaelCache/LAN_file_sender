#pragma once

#include <QAbstractTableModel>
#include <QVector>

#include "server/remote_server.h"

class ReceiverModel : public QAbstractTableModel {
 private:
  /* data */
 public:
  ReceiverModel(QObject *parent = nullptr);
  ~ReceiverModel();

  enum class Column : int { IP = 0, Name, OS, Count };

  bool contains(QHostAddress);
  void add(RemoteServer *);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 private:
  QSet<QString> m_remote_servers_addrs;
  QVector<RemoteServer *> m_remote_servers;
};
