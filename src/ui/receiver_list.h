#pragma once

#include <QTableView>

class ReceiverListView : public QTableView {
  Q_OBJECT
 private:
  /* data */
 public:
  ReceiverListView(QWidget *parent = nullptr);
  ~ReceiverListView();
};
