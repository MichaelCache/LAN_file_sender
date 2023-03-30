#pragma once

#include <QFile>
#include <QHostAddress>
#include <QTcpServer>
#include <QThreadPool>
#include <QByteArray>

#include "package_type.h"
#include "send_task.h"

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

  void onReadyRead();

 protected:
  void incomingConnection(qintptr socketDescriptor);

 private:
  QByteArray preparePackage(PackageType type, QByteArray data = QByteArray());
  void processPackage(PackageType, QByteArray&);

  QThreadPool* m_pool;
  QTcpSocket* m_receive_socket;
  QTcpSocket* m_send_socket;
  QFile* m_send_file{nullptr};
  qint64 m_file_size{0};
  qint64 m_byte_remain{0};

  QByteArray m_receive_buffer;
  QFile* m_receive_file{nullptr};
  QVector<SendTask*> m_senders;
};
