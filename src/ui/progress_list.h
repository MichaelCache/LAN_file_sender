#pragma once

#include <QAction>
#include <QMenu>
#include <QTableView>
#include <QUuid>

#include "progress_bar.h"

class ProgressListView : public QTableView {
  Q_OBJECT
 private:
  /* data */
 public:
  ProgressListView(QWidget* parent = nullptr);
  ~ProgressListView();

 Q_SIGNALS:
  void cancelSendTask(QUuid);
  void clearFinished();

 private Q_SLOTS:
  void onReceiverContextMenuRequested(const QPoint&);
  void cancelTask();
  void openDir();

 private:
  ProgressBar* m_progress;

  QMenu* m_right_menu;
  QAction* m_cancel_ac;
  QAction* m_open_dir_ac;
  QAction* m_clear_ac;
  QUuid m_selected_task;
};
