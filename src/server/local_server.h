#pragma once

#include <QHostAddress>
#include <QLocalServer>
#include <QTimer>
#include <QUdpSocket>

#include "remote_server.h"

class LocalServer : public QLocalServer {
  Q_OBJECT
 public:
  LocalServer(QObject *parent = nullptr);
  ~LocalServer();

  QString getDefaultDownloadPath();

 public Q_SLOTS:
  void sendBroadcast();

 private Q_SLOTS:
  void receiveBroadcast();

 private:
  void sendHostInfo(QHostAddress);
  QVector<QHostAddress> getBroadcastAddressFromInterfaces();

  QHostAddress m_local_host;
  QUdpSocket m_broadcast_udp;
  quint16 m_bc_port{0};
  QMap<QString, RemoteServer> m_remote_servers;
};
