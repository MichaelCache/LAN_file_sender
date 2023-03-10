#pragma once

#include <QTableView>

class ReceiverListView : public QTableView {
 private:
  /* data */
 public:
  ReceiverListView(QWidget *parent = nullptr);
  ~ReceiverListView();
};
