#pragma once

#include <QAbstractTableModel>
#include <QVector>

#include "host_interface.h"

class HostModel : public HostInterface {
 private:
  /* data */
 public:
  HostModel(QObject *parent = nullptr);
  ~HostModel();

  // QAbstractTableModel
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 public Q_SLOTS:
  void add(RemoteHostInfo) override;
  void remove(RemoteHostInfo) override;

 private:
  bool contains(QHostAddress);

  QSet<QString> m_remote_servers_addrs;
  QVector<RemoteHostInfo> m_remote_servers;
};
