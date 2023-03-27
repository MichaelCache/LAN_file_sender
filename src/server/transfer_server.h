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

 private Q_SLOTS:
  void sendFile(const QString& filename);
  // void onReceiveFile();

 protected:
  void incomingConnection(qintptr socketDescriptor);

 private:
  QThreadPool* m_pool;
  QTcpSocket* m_receive_socket;
  QTcpSocket* m_send_socket;
};
