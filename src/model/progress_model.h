#pragma once

#include <QAbstractTableModel>
#include <QMutex>

#include "transfer_info.h"

class ProgressModel : public QAbstractTableModel {
 private:
  /* data */
 public:
  ProgressModel(QObject *parent = nullptr);
  ~ProgressModel();

  // QAbstractTableModel
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 public Q_SLOTS:
  void add(const TransferInfo &);
  void remove(const TransferInfo &);
  void update(const TransferInfo &);

 private:
  void removeMarked();

  QVector<TransferInfo> m_tasks;
  QVector<TransferInfo> m_mark_remove;
  QMutex m_lock;
};
