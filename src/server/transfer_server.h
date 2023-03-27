#pragma once

#include <QTcpServer>
#include <QThreadPool>

class TransferServer : public QTcpServer {
  Q_OBJECT
 private:
  /* data */
 public:
  TransferServer(/* args */);
  ~TransferServer();

  void sendFile(const QString& filename);

 private Q_SLOTS:
  void onReceiveFile();

 protected:
  void incomingConnection(qintptr socketDescriptor);

 private:
  QThreadPool* m_pool;
  QTcpSocket* m_receive_socket;
};
