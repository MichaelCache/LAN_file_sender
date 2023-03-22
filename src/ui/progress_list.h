#pragma once

#include <QTableView>
#include "progress_bar.h"

class ProgressListView : public QTableView {
 private:
  /* data */
 public:
  ProgressListView(QWidget* parent = nullptr);
  ~ProgressListView();

 private:
  ProgressBar* m_progress;
};
