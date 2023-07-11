#pragma once

#include <QAbstractTableModel>

#include "transfer_info.h"

class ProgressInterface : public QAbstractTableModel {
  Q_OBJECT
 public:
  using QAbstractTableModel::QAbstractTableModel;
 public Q_SLOTS:
  virtual void add(const QVector<TransferInfo> &) = 0;
  virtual void remove(const QVector<TransferInfo> &) = 0;
  virtual void update(const QVector<TransferInfo> &) = 0;
  virtual void clear() = 0;
};