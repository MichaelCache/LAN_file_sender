#pragma once

#include <QAbstractTableModel>

#include "server/transfer_task.h"


class ProgressModel : public QAbstractTableModel {
 private:
  /* data */
 public:
  ProgressModel(QObject *parent = nullptr);
  ~ProgressModel();

  enum class Column : int {
    DestIP = 0,
    FileName,
    FileSize,
    State,
    Progress,
    Count
  };

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 private:
  QVector<TransferTask> m_tasks;
};
