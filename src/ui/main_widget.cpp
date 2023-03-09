#include "main_widget.h"

MainWidget::MainWidget(QWidget* parent) : QWidget(parent) {
  m_hlayout = new QHBoxLayout(this);

  m_receiver_list = new ReceiverListView();
  m_progress_list = new ProgressListView();
  m_hlayout->addWidget(m_receiver_list, 3);
  m_hlayout->addWidget(m_progress_list, 6);
}

MainWidget::~MainWidget() {}