#pragma once

#include <QAction>
#include <QMenu>
#include <QTableView>
#include <QUuid>

#include "progress_bar.h"
#include "server/transfer_info.h"

class SendProgressListView : public QTableView {
  Q_OBJECT
 private:
  /* data */
 public:
  SendProgressListView(QWidget* parent = nullptr);
  ~SendProgressListView() = default;

 Q_SIGNALS:
  void cancelSendTask(QVector<TransferInfo>);

 private Q_SLOTS:
  void onCustomRightMouseButtonPressed(const QPoint&);
  void onCancelButtonClicked();
  void onOpenDirButtonClicked();
  void onClearButtonClicked();

 private:
  ProgressBar* m_progress;

  QMenu* m_right_menu;
  QAction* m_cancel_ac;
  QAction* m_open_dir_ac;
  QAction* m_clear_ac;
  QVector<TransferInfo> m_selected_task;
};
