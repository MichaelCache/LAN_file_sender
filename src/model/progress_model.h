#pragma once

#include <QAbstractTableModel>

#include "transfer_info.h"

class ProgressModel : public QAbstractTableModel {
 private:
  /* data */
 public:
  ProgressModel(QObject *parent = nullptr);
  ~ProgressModel();

  void add(const TransferInfo &);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 private:
  QVector<TransferInfo> m_tasks;
};
