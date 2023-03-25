#pragma once

#include <QRunnable>

class SendTask : public QRunnable {
 private:
  /* data */
 public:
  SendTask(/* args */);
  ~SendTask();
  virtual void run() override;
};
