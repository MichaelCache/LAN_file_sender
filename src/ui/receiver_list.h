#pragma once

#include <QAction>
#include <QFileDialog>
#include <QHostAddress>
#include <QMenu>
#include <QTableView>
#include <QStringList>

class ReceiverListView : public QTableView {
  Q_OBJECT

 public:
  ReceiverListView(QWidget* parent = nullptr);
  ~ReceiverListView();

 Q_SIGNALS:
  void sendFile(const QString& filename, const QHostAddress& dst);

 private Q_SLOTS:
  void onReceiverContextMenuRequested(const QPoint&);
  void onSendFile(const QStringList&);

 private:
  void resetFileDialog();
  QFileDialog* m_file_dialog;

  QMenu* m_right_menu;
  QAction* m_send_ac;
};
