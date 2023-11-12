#pragma once

#include <QObject>

#include "control_server.h"
#include "host_broadcaster.h"
#include "transfer_server.h"

class MainServer : public QObject {
  Q_OBJECT
 public:
  MainServer(QObject* parent = nullptr);
  ~MainServer();

  void start();
  void stop();
  const QVector<QHostAddress>& hostIp();

 public Q_SLOTS:
  // slots as sender
  void onSendFile(QVector<TransferInfo> info);
  void onSendCancelFile(QVector<TransferInfo> info);

  // slots as reciver
  void onAcceptFile(QVector<TransferInfo> info);

 Q_SIGNALS:
  // signal from broadcast
  void detectHostOnLine(RemoteHostInfo);
  void detectHostOffline(RemoteHostInfo);

  // signal as sender
  void recieveFileInfo(QVector<TransferInfo> info);
  void sendFileDenied(QVector<TransferInfo> trans_info);
  void updateSendProgress(QVector<TransferInfo>);

  // signal as reciever
  void updateReceiveProgress(QVector<TransferInfo> info);

 private:
  QMutex m_lock;
  QVector<TransferInfo> m_send_wating_task;
  QVector<TransferInfo> m_recieve_wating_task;
  // broadcaster for broadcast host info like name, ip
  HostBroadcaster* m_host_detector;
  // control server for send/recive file tranfer control signal
  ControlServer* m_control_server;
  // transfer server for file data transfer
  TransferServer* m_transfer_server;
};
