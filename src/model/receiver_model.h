#pragma once

#include <QAbstractTableModel>
#include <QVector>
#include <QMutex>

#include "host_info.h"

class ReceiverModel : public QAbstractTableModel {
 private:
  /* data */
 public:
  ReceiverModel(QObject *parent = nullptr);
  ~ReceiverModel();

  // QAbstractTableModel
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 public Q_SLOTS:
  void add(const RemoteHostInfo &);
  void remove(const RemoteHostInfo &);

 private:
  bool contains(QHostAddress);
  
  QSet<QString> m_remote_servers_addrs;
  QVector<RemoteHostInfo> m_remote_servers;
  QMutex m_lock;  
};
