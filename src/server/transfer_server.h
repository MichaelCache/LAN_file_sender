#pragma once

#include <QByteArray>
#include <QFile>
#include <QHostAddress>
#include <QMutex>
#include <QTcpServer>
#include <QThreadPool>

#include "model/progress_model.h"
#include "model/receiver_model.h"
#include "model/transfer_info.h"
#include "package_type.h"
#include "receive_task.h"
#include "send_task.h"


class TransferServer : public QTcpServer {
  Q_OBJECT
 public:
  TransferServer(QObject* parent = nullptr);
  ~TransferServer();

  ProgressModel* progressModel();

 public Q_SLOTS:
  void sendFile(const QString& filename, const QHostAddress& dst);
  // void onReceiveFile();

 protected:
  void incomingConnection(qintptr socketDescriptor);

 private:
  ProgressModel* m_progress_model;

  QVector<SendTask*> m_senders;
  QVector<ReceiveTask*> m_receivers;
};
