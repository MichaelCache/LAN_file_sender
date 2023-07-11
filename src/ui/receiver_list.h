#pragma once

#include <QAction>
#include <QFileDialog>
#include <QHostAddress>
#include <QMenu>
#include <QStringList>
#include <QTableView>

#include "server/transfer_info.h"

class ReceiverListView : public QTableView {
  Q_OBJECT

 public:
  ReceiverListView(QWidget* parent = nullptr);
  ~ReceiverListView();

 Q_SIGNALS:
  void sendFile(const QVector<TransferInfo>& info);

 private Q_SLOTS:
  void onReceiverContextMenuRequested(const QPoint&);
  void onSendFile(const QStringList&);
  void openFileDialog();

 private:
  QMenu* m_right_menu;
  QAction* m_send_ac;
};
