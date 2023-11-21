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
  ~RecieveProgressListView() = default;

 Q_SIGNALS:
  void cancelSendTask(QVector<TransferInfo>);
  void acceptSendTask(QVector<TransferInfo>);
  void rejectSendTask(QVector<TransferInfo>);
  void clearFinished(QVector<TransferInfo>);
  

 private Q_SLOTS:
  void onCustomRightMouseButtonPressed(const QPoint&);

  void onAcceptButtonClicked();
  void onRejectButtonClicked();
  void onCancelButtonClicked();
  void onOpenDirButtonClicked();
  void onClearButtonClicked();

 private:
  ProgressBar* m_progress;

  QMenu* m_right_menu;
  QAction* m_cancel_ac;
  QAction* m_accept_ac;
  QAction* m_reject_ac;
  QAction* m_open_dir_ac;
  QAction* m_clear_ac;
  QVector<TransferInfo> m_selected_task;
};
