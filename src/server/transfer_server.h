#pragma once

#include <QLocalServer>

class TransferServer : public QLocalServer {
 private:
  /* data */
 public:
  TransferServer(QObject *parent = nullptr);
  ~TransferServer();
};
