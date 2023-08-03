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

 Q_SIGNALS:
  // signal from broadcast
  void detectHostOnLine(RemoteHostInfo);
  void detectHostOffline(RemoteHostInfo);

  // signal as sender
  void recieveFileInfo(QVector<TransferInfo> info);
  void sendFileDenied(QVector<TransferInfo> trans_info);
  void updateSendProgress(TransferInfo);

  // signal as reciever
  void updateReceiveProgress(TransferInfo);

 private:
  HostBroadcaster* m_host_detector;
  ControlServer* m_control_server;
  TransferServer* m_transfer_server;
};
