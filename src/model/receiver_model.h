#pragma once

#include <QStandardItemModel>

class ReceiverModel : public QStandardItemModel {
 private:
  /* data */
 public:
  ReceiverModel(QObject *parent = nullptr);
  ~ReceiverModel();
};


