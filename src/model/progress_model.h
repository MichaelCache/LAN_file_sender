#pragma once

#include <QMutex>
#include <QVector>

#include "progress_interface.h"

class ProgressModel : public ProgressInterface {
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
  virtual void add(QVector<TransferInfo>) override;
  virtual void remove(QVector<TransferInfo>) override;
  virtual void update(QVector<TransferInfo>) override;
  virtual const TransferInfo &get(QUuid id) override;
  virtual void clear() override;

 protected:
  QVector<TransferInfo> m_tasks;
  QMutex m_lock;
};
