#include "main_widget.h"

#include <QMessageBox>

#include "config.h"
#include "setting.h"

MainWidget::MainWidget(QWidget* parent) : QWidget(parent) {
  m_server = new MainServer(this);

  m_host_model = new ReceiverModel(this);
  m_send_task_model = new ProgressModel(this);
  m_receive_task_model = new ProgressModel(this);

  m_host_view = new ReceiverListView(this);
  m_send_progress_view = new ProgressListView(this);
  m_receive_progress_view = new ProgressListView(this);

  m_host_view->setModel(m_host_model);
  m_send_progress_view->setModel(m_send_task_model);
  m_receive_progress_view->setModel(m_receive_task_model);

  // connect host info data
  connect(m_server, &MainServer::detectHostOnLine, m_host_model,
          &HostInterface::add);
  connect(m_server, &MainServer::detectHostOffline, m_host_model,
          &HostInterface::remove);

  // connect send file as sender
  connect(m_host_view, &ReceiverListView::sendFile, m_server,
          &MainServer::onSendFile);
  connect(m_host_view, &ReceiverListView::sendFile, m_send_task_model,
          &ProgressInterface::add);
//   connect(m_server, &MainServer::updateSendProgress, m_send_task_model,
//           &ProgressInterface::update);
  connect(m_server, &MainServer::sendFileDenied, m_send_task_model,
          &ProgressInterface::update);
  connect(m_send_progress_view, &ProgressListView::cancelSendTask, m_server,
          &MainServer::onSendCancelFile);

  // connect receive file as receiver
  connect(m_server, &MainServer::recieveFileInfo, m_receive_task_model,
          &ProgressInterface::add);

  // ui
  m_progress_layout = new QVBoxLayout();
  m_progress_layout->addWidget(m_send_progress_view);
  m_progress_layout->addWidget(m_receive_progress_view);

  m_host_progress_layout = new QHBoxLayout();
  m_host_progress_layout->addWidget(m_host_view, 3);
  m_host_progress_layout->addLayout(m_progress_layout, 6);

  m_localhostname = new QLabel(this);
  m_localhostname->setText("Host Name: " + Setting::ins().m_hostname);
  connect(&Setting::ins(), &Setting::updateSettings, this,
          &MainWidget::onUpdateSettings);

  m_localhostip = new QLabel(this);
  QString local_ips = "Host IP: ";
  for (auto&& i : m_server->hostIp()) {
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
  m_main_layout->addLayout(m_host_progress_layout);
  m_main_layout->addLayout(m_bottom_labels_layout);

  setLayout(m_main_layout);

  m_server->start();
}

MainWidget::~MainWidget() {}

void MainWidget::onClose() { m_server->stop(); }

void MainWidget::onUpdateSettings() {
  m_localhostname->setText("Host Name: " + Setting::ins().m_hostname);
}
