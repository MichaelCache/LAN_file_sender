#pragma once

#include <QHostAddress>
#include <QTcpServer>
#include <QThreadPool>
#include <QFile>

#include "package_type.h"

class TransferServer : public QTcpServer {
  Q_OBJECT
 private:
  /* data */
 public:
  TransferServer(QObject* parent = nullptr);
  ~TransferServer();

 public Q_SLOTS:
  void sendFile(const QString& filename, const QHostAddress& dst);
  // void onReceiveFile();

 private Q_SLOTS:
  void onBytesWritten(qint64 byte);
  void onConnected();
  void onDisconnected();

 protected:
  void incomingConnection(qintptr socketDescriptor);

 private:
  QByteArray preparePackage(PackageType type, QByteArray data);

  QThreadPool* m_pool;
  QTcpSocket* m_receive_socket;
  QTcpSocket* m_send_socket;
  QFile* m_send_file{nullptr};
};
