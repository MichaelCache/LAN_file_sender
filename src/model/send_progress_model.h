#pragma once

#include <QAbstractTableModel>
#include <QMutex>

#include "progress_model.h"
#include "server/transfer_info.h"

/**
 * @brief Model hold data for transfer task
 *
 */
class SendProgressModel : public ProgressModel {
 public:
  SendProgressModel(QObject *parent = nullptr);
  ~SendProgressModel();

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
};
