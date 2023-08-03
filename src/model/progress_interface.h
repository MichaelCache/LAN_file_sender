#pragma once

#include <QAbstractTableModel>

#include "server/transfer_info.h"

class ProgressInterface : public QAbstractTableModel {
  Q_OBJECT
 public:
  using QAbstractTableModel::QAbstractTableModel;
 public Q_SLOTS:
  virtual void add(QVector<TransferInfo>) = 0;
  virtual void remove(QVector<TransferInfo>) = 0;
  virtual void update(QVector<TransferInfo>) = 0;
  virtual void clear() = 0;
};