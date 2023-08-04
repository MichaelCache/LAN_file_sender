#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>


#include "model/host_model.h"
#include "progress_list.h"
#include "host_list.h"
#include "server/host_broadcaster.h"
#include "server/main_server.h"

class MainWidget : public QWidget {
  Q_OBJECT
 public:
  MainWidget(QWidget *parent = nullptr);
  ~MainWidget();

  void onClose();

 private Q_SLOTS:
  void onUpdateSettings();

 private:
  // ui
  QVBoxLayout *m_main_layout;
  QHBoxLayout *m_host_progress_layout;
  QVBoxLayout *m_progress_layout;
  QHBoxLayout *m_bottom_labels_layout;
  QLabel *m_localhostname;
  QLabel *m_localhostip;

  // data view
  HostListView *m_host_view;
  SendProgressListView *m_send_progress_view;
  SendProgressListView *m_receive_progress_view;

  // servers;
  MainServer *m_server;

  // data model
  HostInterface *m_host_model;
  ProgressInterface *m_send_task_model;
  ProgressInterface *m_receive_task_model;
};
