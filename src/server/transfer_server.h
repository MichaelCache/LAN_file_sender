#pragma once

#include <QByteArray>
#include <QFile>
#include <QHostAddress>
#include <QTcpServer>
#include <QThreadPool>

#include "package_type.h"
#include "send_task.h"
#include "receive_task.h"

class TransferServer : public QTcpServer {
  Q_OBJECT
 public:
  TransferServer(QObject* parent = nullptr);
  ~TransferServer();

 public Q_SLOTS:
  void sendFile(const QString& filename, const QHostAddress& dst);
  // void onReceiveFile();

 protected:
  void incomingConnection(qintptr socketDescriptor);

 private:
  QVector<SendTask*> m_senders;
  QVector<ReceiveTask*> m_receivers;
};
