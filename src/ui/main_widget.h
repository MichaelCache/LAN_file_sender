#pragma once

#include <QHBoxLayout>
#include <QWidget>

#include "model/progress_model.h"
#include "model/receiver_model.h"
#include "progress_list.h"
#include "receiver_list.h"
#include "server/local_server.h"

class MainWidget : public QWidget {
 public:
  MainWidget(QWidget *parent = nullptr);
  ~MainWidget();

 private:
  /* data */
  QHBoxLayout *m_hlayout;
  ReceiverListView *m_receiver_view;
  ReceiverModel *m_receiver_model;
  ProgressListView *m_progress_view;
  ProgressModel *m_progress_model;

  LocalServer* m_local_server;
};
