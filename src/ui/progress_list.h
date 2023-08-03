#pragma once

#include <QAction>
#include <QMenu>
#include <QTableView>
#include <QUuid>

#include "progress_bar.h"
#include "server/transfer_info.h"

class ProgressListView : public QTableView {
  Q_OBJECT
 private:
  /* data */
 public:
  ProgressListView(QWidget* parent = nullptr);
  ~ProgressListView();

 Q_SIGNALS:
  void cancelSendTask(QVector<TransferInfo>);
  void clearFinished();

 private Q_SLOTS:
  void onCustomRightMouseButtonPressed(const QPoint&);
  void onCancelButtonClickedk();
  void openDir();

 private:
  ProgressBar* m_progress;

  QMenu* m_right_menu;
  QAction* m_cancel_ac;
  QAction* m_open_dir_ac;
  QAction* m_clear_ac;
  QVector<TransferInfo> m_selected_task;
  QString m_selected_file_path;
};
