#pragma once

#include <QAbstractTableModel>
#include <QMutex>

#include "progress_model.h"
#include "server/transfer_info.h"

/**
 * @brief Model hold data for transfer task
 *
 */
class RecieveProgressModel : public ProgressModel {
 public:
  RecieveProgressModel(QObject *parent = nullptr);
  ~RecieveProgressModel();

  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
};
