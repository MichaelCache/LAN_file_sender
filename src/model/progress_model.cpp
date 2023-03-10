#include "progress_model.h"

#include "column.h"

ProgressModel::ProgressModel(QObject *parent) : QStandardItemModel(parent) {
  setColumnCount(5);
  setHeaderData((int)ProgressColumn::DestIP, Qt::Horizontal, "Dest");
  setHeaderData((int)ProgressColumn::FileName, Qt::Horizontal, "FileName");
  setHeaderData((int)ProgressColumn::FileSize, Qt::Horizontal, "Size");
  setHeaderData((int)ProgressColumn::State, Qt::Horizontal, "State");
  setHeaderData((int)ProgressColumn::Progress, Qt::Horizontal, "Progress");
}

ProgressModel::~ProgressModel() {}
