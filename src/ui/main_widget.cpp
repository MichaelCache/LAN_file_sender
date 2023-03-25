#include "main_widget.h"

#include <QMessageBox>

#include "config.h"

// #include "model/transfer_task.h"

MainWidget::MainWidget(QWidget* parent) : QWidget(parent) {
  m_host_broadcaster = new HostDetector(this);

  m_receiver_view = new ReceiverListView(this);
  m_receiver_model = new ReceiverModel(this);
  m_receiver_view->setModel(m_receiver_model);

  connect(m_host_broadcaster, &HostDetector::addHost, m_receiver_model,
          &ReceiverModel::add);
  connect(m_host_broadcaster, &HostDetector::removeHost, m_receiver_model,
          &ReceiverModel::remove);
  m_host_broadcaster->broadcast(MsgType::New);

  m_progress_view = new ProgressListView(this);
  m_progress_model = new ProgressModel(this);
  m_progress_view->setModel(m_progress_model);

  // for test
  auto task = TransferInfo();
  task.m_progress = 60;
  m_progress_model->add(task);

  m_hlayout = new QHBoxLayout(this);
  m_hlayout->addWidget(m_receiver_view, 3);
  m_hlayout->addWidget(m_progress_view, 6);

  // m_local_server = new LocalServer(this);
}

MainWidget::~MainWidget() {}

void MainWidget::onClose() { m_host_broadcaster->broadcast(MsgType::Delete); }