#pragma once

#include <QAbstractTableModel>

#include "host_info.h"

namespace RemoteClient {
enum class Column : int { IP = 0, Name, OS, Count };
}


class HostInterface : public QAbstractTableModel {
  Q_OBJECT
 public:
  using QAbstractTableModel::QAbstractTableModel;

 public Q_SLOTS:
  virtual void add(RemoteHostInfo) = 0;
  virtual void remove(RemoteHostInfo) = 0;
};
