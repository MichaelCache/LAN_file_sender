#pragma once

#include <QFile>
#include <QTcpSocket>
#include <QThread>

class SendTask : public QThread {
 private:
  /* data */
 public:
  SendTask(/* args */);
  ~SendTask();
  virtual void run() override;

 private:
  QFile* m_send_file{nullptr};
};
