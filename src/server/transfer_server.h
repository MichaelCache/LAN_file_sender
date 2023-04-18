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
#include "receive_task.h"
#include "send_task.h"

class TransferServer : public QTcpServer {
  Q_OBJECT
 public:
  TransferServer(QObject* parent = nullptr);
  ~TransferServer();

  ProgressModel* progressModel();

 public Q_SLOTS:
  void onSendFile(const QString& filename, const QHostAddress& dst);
  void onCancelSend(QUuid);

 Q_SIGNALS:
  // void cancelSend(const TransferInfo&);
  // void onReceiveFile();
 private Q_SLOTS:
  // void finishSend();
  // void finishReceive();
  void removeSend(QUuid);
  void removeReceive(QUuid taskid); 

 protected:
  void incomingConnection(qintptr socketDescriptor);

 private:
  void appendSend(SendTask*);
  void appendReceive(ReceiveTask* receiver);

  ProgressModel* m_progress_model;
  QMutex m_lock;

  QMap<QUuid, SendTask*> m_senders;
  QMap<QUuid, ReceiveTask*> m_receivers;
  QVector<SendTask*> m_sender_wait_queue;
  QVector<ReceiveTask*> m_receiver_wait_queue;
};
