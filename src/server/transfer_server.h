#pragma once

#include <QByteArray>
#include <QFile>
#include <QHostAddress>
#include <QMutex>
#include <QTcpServer>
#include <QThreadPool>

#include "fileinfo.h"
#include "model/progress_model.h"
#include "model/receiver_model.h"
#include "model/progress_interface.h"
#include "receive_task.h"
#include "send_task.h"
#include "transfer_info.h"

class TransferServer : public QTcpServer {
  Q_OBJECT
 public:
  TransferServer(QObject* parent = nullptr);
  ~TransferServer();

  void start();
  void stop();

 public Q_SLOTS:
  void onSendFile(QVector<TransferInfo> info);
  void onCancelSend(QVector<TransferInfo> info);

 private Q_SLOTS:
  void removeSend(const QUuid& task_id);
  void removeReceive(const QUuid& task_id);

 Q_SIGNALS:
  void updateSendProgress(const TransferInfo&);
  void updateReceiveProgress(const TransferInfo&);
  void newReceiveTaskCreated(ReceiveTask*);
  void newSendTaskCreated(SendTask*);

 protected:
  void incomingConnection(qintptr socketDescriptor);

 private:
  using TaskId = typename TransferInfo::TaskId;

  void appendSend(const TaskId&, SendTask*);
  void appendReceive(const TaskId&, ReceiveTask* receiver);
  void bumpSendQueue();
  void bumpReceiveQueue();

  QMutex m_lock;

  QMap<TaskId, SendTask*> m_senders;
  QMap<TaskId, ReceiveTask*> m_receivers;
  QVector<QPair<TaskId, SendTask*>> m_sender_wait_queue;
  QVector<QPair<TaskId, ReceiveTask*>> m_receiver_wait_queue;
};
