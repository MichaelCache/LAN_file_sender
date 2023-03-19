#pragma once

#include <QAbstractTableModel>
#include "server/remote_server.h"

class ReceiverModel : public QAbstractTableModel {
 private:
  /* data */
 public:
  ReceiverModel(QObject *parent = nullptr);
  ~ReceiverModel();

  enum class Column : int { IP = 0, Name, OS };

  bool contains(QHostAddress);
  void add(RemoteServer *);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 private:
  QMap<QString, RemoteServer *> m_remote_servers;
};
