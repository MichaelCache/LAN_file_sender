#pragma once

#include <QAbstractTableModel>
#include <QMutex>

#include "progress_interface.h"
#include "server/transfer_info.h"

/**
 * @brief Model hold data for transfer task
 *
 */
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
  virtual void add(const QVector<TransferInfo> &) override;
  virtual void remove(const QVector<TransferInfo> &) override;
  virtual void update(const QVector<TransferInfo> &) override;
  virtual void clear() override;

 private:
  QVector<TransferInfo> m_tasks;
  QMutex m_lock;
};
