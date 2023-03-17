#include "receiver_model.h"
#include "column.h"

ReceiverModel::ReceiverModel(QObject *parent) : QStandardItemModel(parent) {
  setColumnCount(3);
  setHeaderData(0, Qt::Horizontal, "IP");
  setHeaderData(1, Qt::Horizontal, "Name");
  setHeaderData(2, Qt::Horizontal, "OS");
}

ReceiverModel::~ReceiverModel() {}