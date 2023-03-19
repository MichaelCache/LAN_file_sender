#pragma once

#include <QHostAddress>
#include <QLocalServer>
#include <QTimer>
#include <QUdpSocket>

#include "model/receiver_model.h"
#include "remote_server.h"


class LocalServer : public QLocalServer {
  Q_OBJECT
 public:
  LocalServer(QObject* parent = nullptr);
  ~LocalServer();

  QString getDefaultDownloadPath();

  ReceiverModel* receivers();

 public Q_SLOTS:
  void sendBroadcast();

 private Q_SLOTS:
  void receiveBroadcast();

 private:
  void sendHostInfo(QHostAddress);
  QVector<QHostAddress> getBroadcastAddressFromInterfaces();

  QHostAddress m_local_host;
  QUdpSocket m_broadcast_udp;
  ReceiverModel* m_receiver;
  // QMap<QString, RemoteServer*> m_remote_servers;
};
