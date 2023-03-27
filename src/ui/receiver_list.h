#pragma once

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QTableView>


class ReceiverListView : public QTableView {
  Q_OBJECT

 public:
  ReceiverListView(QWidget* parent = nullptr);
  ~ReceiverListView();

 Q_SIGNALS:
  void sendFile(const QString&);

 private Q_SLOTS:
  void onReceiverContextMenuRequested(const QPoint&);
  void onSendFile();

 private:
  QFileDialog* m_file_dialog;

  QMenu* m_right_menu;
  QAction* m_send_ac;
};
