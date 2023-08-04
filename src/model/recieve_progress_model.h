#pragma once

#include <QAbstractTableModel>
#include <QMutex>

#include "progress_interface.h"
#include "server/transfer_info.h"



/**
 * @brief Model hold data for transfer task
 *
 */
class RecieveProgressModel : public ProgressInterface {
 public:
  RecieveProgressModel(QObject *parent = nullptr);
  ~RecieveProgressModel();

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
  virtual void clear() override;

 private:
  QVector<TransferInfo> m_tasks;
  QMutex m_lock;
};
