#include "main_widget.h"

#include <QMessageBox>

#include "config.h"
#include "setting.h"

MainWidget::MainWidget(QWidget* parent) : QWidget(parent) {
  m_host_detector = new HostDetector(this);
  m_file_transfer = new TransferServer(this);

  m_receiver_view = new ReceiverListView(this);
  m_receiver_view->setModel(m_host_detector->receiverModel());

  m_host_detector->broadcast(MsgType::New);

  connect(m_receiver_view, &ReceiverListView::sendFile, m_file_transfer,
          &TransferServer::sendFile);
  m_file_transfer->listen(QHostAddress::Any, Setting::ins().m_file_trans_port);

  m_progress_view = new ProgressListView(this);
  m_progress_view->setModel(m_file_transfer->progressModel());

  m_hlayout_level_1 = new QHBoxLayout(this);
  m_hlayout_level_1->addWidget(m_receiver_view, 3);
  m_hlayout_level_1->addWidget(m_progress_view, 6);

#if 0
  m_vlayout_level_0 = new QVBoxLayout(this);
  m_localhost_info = new QLabel(this);
  m_vlayout_level_0->addLayout(m_hlayout_level_1);
#endif

  // m_localhost_info->text = QString("HostName %1  IP: ").arg(m_de)
  // m_vlayout_level_0->addWidget();

  // for progress test
  //   auto task = TransferInfo();
  //   task.m_progress = 60;
  //   m_progress_model->add(task);

  //   // for receiver test
  //   RemoteHostInfo receiver{QHostAddress("192.168.10.31"), "Num", "Linux"};
  //   m_receiver_model->add(receiver);
}

MainWidget::~MainWidget() {}

void MainWidget::onClose() { m_host_detector->broadcast(MsgType::Delete); }