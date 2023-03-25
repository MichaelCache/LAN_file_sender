#pragma once

#include <QRunnable>
#include <QTcpSocket>

class ReceiveTask : public QRunnable {
 private:
  /* data */
 public:
  ReceiveTask();
  ~ReceiveTask();

  virtual void run() override;

 private:
  QTcpSocket* m_receive_tcp;
};
