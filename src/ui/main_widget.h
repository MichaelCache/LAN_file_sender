#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "model/progress_model.h"
#include "model/receiver_model.h"
#include "progress_list.h"
#include "receiver_list.h"
#include "server/host_detector.h"
#include "server/transfer_server.h"

class MainWidget : public QWidget {
  Q_OBJECT
 public:
  MainWidget(QWidget *parent = nullptr);
  ~MainWidget();

 public Q_SLOTS:
  void onClose();

 private:
  QVBoxLayout *m_vlayout_level_0;
  QHBoxLayout *m_hlayout_level_1;
  ReceiverListView *m_receiver_view;
  ProgressListView *m_progress_view;

  QLabel *m_localhost_info;

  // servers;
  HostDetector *m_host_detector;
  TransferServer *m_file_transfer;
};
