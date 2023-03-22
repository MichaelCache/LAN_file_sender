#include "main_widget.h"

#include "server/transfer_task.h"

MainWidget::MainWidget(QWidget* parent) : QWidget(parent) {
  m_local_server = new LocalServer(this);

  m_receiver_view = new ReceiverListView(this);
  // m_receiver_model = new ReceiverModel(this);
  m_receiver_view->setModel(m_local_server->receivers());
  m_local_server->sendBroadcast();

  m_progress_view = new ProgressListView(this);
  m_progress_model = new ProgressModel(this);
  m_progress_view->setModel(m_progress_model);

  // for test
  auto task = TransferTask();
  task.m_progress = 60;
  m_progress_model->add(task);

  m_hlayout = new QHBoxLayout(this);
  m_hlayout->addWidget(m_receiver_view, 3);
  m_hlayout->addWidget(m_progress_view, 6);

  // m_local_server = new LocalServer(this);
}

MainWidget::~MainWidget() {}