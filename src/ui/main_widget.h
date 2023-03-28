#pragma once

#include <QHBoxLayout>
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
  /* data */
  QHBoxLayout *m_hlayout;
  ReceiverListView *m_receiver_view;
  ReceiverModel *m_receiver_model;
  ProgressListView *m_progress_view;
  ProgressModel *m_progress_model;

  HostDetector *m_host_broadcaster;
  TransferServer* m_file_transfer;
};
