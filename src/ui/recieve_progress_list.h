#pragma once

#include <QAction>
#include <QMenu>
#include <QTableView>
#include <QUuid>

#include "progress_bar.h"
#include "server/transfer_info.h"

class RecieveProgressListView : public QTableView {
  Q_OBJECT
 private:
  /* data */
 public:
  RecieveProgressListView(QWidget* parent = nullptr);
  ~RecieveProgressListView();

 Q_SIGNALS:
  void cancelSendTask(QVector<TransferInfo>);
  void acceptSendTask(QVector<TransferInfo>);
  void clearFinished();

 private Q_SLOTS:
  void onCustomRightMouseButtonPressed(const QPoint&);
  void onCancelButtonClickedk();
  void onAcceptButtonClickedk();
  void openDir();

 private:
  ProgressBar* m_progress;

  QMenu* m_right_menu;
  QAction* m_cancel_ac;
  QAction* m_accept_ac;
  QAction* m_open_dir_ac;
  QAction* m_clear_ac;
  QVector<TransferInfo> m_selected_task;
};
