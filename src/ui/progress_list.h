#pragma once

#include <QTableView>

class ProgressListView : public QTableView {
 private:
  /* data */
 public:
  ProgressListView(QWidget *parent = nullptr);
  ~ProgressListView();
};


