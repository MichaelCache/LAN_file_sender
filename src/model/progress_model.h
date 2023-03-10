#pragma once

#include <QStandardItemModel>

class ProgressModel : public QStandardItemModel {
 private:
  /* data */
 public:
  ProgressModel(QObject *parent = nullptr);
  ~ProgressModel();
};
