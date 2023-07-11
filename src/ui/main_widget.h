#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "model/progress_model.h"
#include "model/receiver_model.h"
#include "progress_list.h"
#include "receiver_list.h"
#include "server/host_broadcaster.h"
#include "server/main_server.h"

class MainWidget : public QWidget {
  Q_OBJECT
 public:
  MainWidget(QWidget *parent = nullptr);
  ~MainWidget();

 public Q_SLOTS:
  void onClose();

 private Q_SLOTS:
  void onUpdateSettings();

 private:
  QVBoxLayout *m_main_layout;
  QHBoxLayout *m_receiver_progress_layout;
  QHBoxLayout *m_bottom_labels_layout;
  ReceiverListView *m_receiver_view;
  ProgressListView *m_progress_view;

  QLabel *m_localhostname;
  QLabel *m_localhostip;

  // servers;

  MainServer *m_server;
};
