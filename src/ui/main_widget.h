#pragma once

#include <QWidget>
#include <QHBoxLayout>

#include "progress_list.h"
#include "receiver_list.h"


class MainWidget : public QWidget {
 public:
  MainWidget(QWidget *parent = nullptr);
  ~MainWidget();

 private:
  /* data */
  QHBoxLayout *m_hlayout;
  ReceiverListView *m_receiver_list;
  ProgressListView *m_progress_list;
};
