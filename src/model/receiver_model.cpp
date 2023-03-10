#include "receiver_model.h"
#include "column.h"

ReceiverModel::ReceiverModel(QObject *parent) : QStandardItemModel(parent) {
  setColumnCount(2);
  setHeaderData(0, Qt::Horizontal, "IP");
  setHeaderData(1, Qt::Horizontal, "comment");
}

ReceiverModel::~ReceiverModel() {}