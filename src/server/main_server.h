#pragma once

#include <QObject>
#include <unordered_set>

#include "control_server.h"
#include "host_broadcaster.h"
#include "transfer_server.h"

class MainServer : public QObject {
  Q_OBJECT
 public:
  MainServer(QObject* parent = nullptr);
  ~MainServer() = default;

  void start();
  void stop();
  const QVector<QHostAddress>& hostIp();

 public Q_SLOTS:
  // slots as sender
  void senderSendFile(QVector<TransferInfo> info);
  void senderSendFileBeCanceled(QVector<TransferInfo> info);

  // slots as reciver
  void reciverAcceptFile(QVector<TransferInfo> info);
  void reciverRejectFile(QVector<TransferInfo> info);
  void reciverCancelFile(QVector<TransferInfo> info);

 private Q_SLOTS:
  // slots as sender
  void sendFileToRemote(QVector<TransferInfo> info);
  void clearRejectedSend(QVector<TransferInfo> info);
  void senderSendFileFinished(QVector<TransferInfo> info);

 Q_SIGNALS:
  // signal from broadcast
  void detectHostOnLine(RemoteHostInfo);
  void detectHostOffline(RemoteHostInfo);

  // signal from sender
  void updateSendProgress(QVector<TransferInfo>);

  // signal from reciever
  void updateReciveProgress(QVector<TransferInfo> info);

 private:
  // QMutex m_lock;
  // QMap<QUuid, TransferInfo> m_send_pending_task;
  struct TransferInfoHash {
    size_t operator()(const TransferInfo& info) const {
      return qHash(info.id());
    }
  };
  std::unordered_set<TransferInfo, TransferInfoHash> m_send_pending_task;
  // broadcaster for broadcast host info like name, ip
  HostBroadcaster* m_host_detector;
  // control server for send/recive file tranfer control signal
  ControlServer* m_control_server;
  // transfer server for file data transfer
  TransferServer* m_transfer_server;
};
