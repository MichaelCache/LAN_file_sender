#pragma once

#include <QTableView>
#include "progress_bar.h"

class ProgressListView : public QTableView {
  Q_OBJECT
 private:
  /* data */
 public:
  ProgressListView(QWidget* parent = nullptr);
  ~ProgressListView();

 private:
  ProgressBar* m_progress;
};
