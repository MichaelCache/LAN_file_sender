#include "main_widget.h"

MainWidget::MainWidget(QWidget* parent) : QWidget(parent) {
  m_receiver_view = new ReceiverListView(this);
  m_receiver_model = new ReceiverModel(this);
  m_receiver_view->setModel(m_receiver_model);

  m_progress_view = new ProgressListView(this);
  m_progress_model = new ProgressModel(this);
  m_progress_view->setModel(m_progress_model);

  m_hlayout = new QHBoxLayout(this);
  m_hlayout->addWidget(m_receiver_view, 3);
  m_hlayout->addWidget(m_progress_view, 6);

  m_local_server = new LocalServer(this);
  m_local_server->sendBroadcast();
}

MainWidget::~MainWidget() {}