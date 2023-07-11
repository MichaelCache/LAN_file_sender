#pragma once

#include <QAbstractTableModel>
#include "host_info.h"

class HostInterface : public QAbstractTableModel {
  Q_OBJECT
 public:
  using QAbstractTableModel::QAbstractTableModel;

 public Q_SLOTS:
  virtual void add(const RemoteHostInfo &) = 0;
  virtual void remove(const RemoteHostInfo &) = 0;
};
