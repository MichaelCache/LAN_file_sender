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

 public Q_SLOTS:
  void onSendFileInfos(const QVector<TransferInfo>& info,
                       const QHostAddress& dst);
  void onSendFiles(const QVector<TransferInfo>& files);
  void onAcceptSend(const QVector<TransferInfo>& files);

 Q_SIGNALS:
  // single from broadcast
  void detectNewHost(const RemoteHostInfo&);
  void detectHostOffline(const RemoteHostInfo&);

  void recieveFileInfo(const QVector<TransferInfo>& info);
  void updateSendProgress(const TransferInfo&);
  void updateReceiveProgress(const TransferInfo&);
  void newReceiveTaskCreated(ReceiveTask*);
  void newSendTaskCreated(SendTask*);

 private:
  HostBroadcaster* m_host_detector;
  ControlServer* m_control_server;
  TransferServer* m_transfer_server;
};
