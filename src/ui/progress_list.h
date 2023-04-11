#pragma once

#include <QAction>
#include <QMenu>
#include <QTableView>

#include "progress_bar.h"

class ProgressListView : public QTableView {
  Q_OBJECT
 private:
  /* data */
 public:
  ProgressListView(QWidget* parent = nullptr);
  ~ProgressListView();

 Q_SIGNALS:
  void cancelSendTask(qintptr);

 private Q_SLOTS:
  void onReceiverContextMenuRequested(const QPoint&);
  void cancelTask();

 private:
  ProgressBar* m_progress;

  QMenu* m_right_menu;
  QAction* m_cancel_ac;
  qintptr m_selected_task{-1};
};
