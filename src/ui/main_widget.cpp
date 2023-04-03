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

  m_receiver_progress_layout = new QHBoxLayout();
  m_receiver_progress_layout->addWidget(m_receiver_view, 3);
  m_receiver_progress_layout->addWidget(m_progress_view, 6);

  m_localhostname = new QLabel(this);
  m_localhostname->setText("Host Name: " + m_host_detector->hostName());

  m_localhostip = new QLabel(this);
  QString local_ips = "Host IP: ";
  for (auto&& i : m_host_detector->hostIp()) {
    local_ips.append(QHostAddress(i.toIPv4Address()).toString() + "/");
  }
  if (local_ips.endsWith("/")) {
    local_ips.chop(1);
  }
  m_localhostip->setText(local_ips);

  m_bottom_labels_layout = new QHBoxLayout();
  m_bottom_labels_layout->addWidget(m_localhostname);
  m_bottom_labels_layout->addStretch(200);
  m_bottom_labels_layout->addWidget(m_localhostip);

  m_main_layout = new QVBoxLayout();
  m_main_layout->addLayout(m_receiver_progress_layout);
  m_main_layout->addLayout(m_bottom_labels_layout);

  setLayout(m_main_layout);
}

MainWidget::~MainWidget() {}

void MainWidget::onClose() { m_host_detector->broadcast(MsgType::Delete); }