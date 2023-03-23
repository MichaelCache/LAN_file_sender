#pragma once

#include <QHostAddress>
#include <QLocalServer>
#include <QTimer>
#include <QUdpSocket>

#include "model/receiver_model.h"
#include "remote_server.h"

enum class MsgType : int { None = 0, New, Update, Reply };

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
  void sendHostInfo(QHostAddress, MsgType);
  QVector<QHostAddress> getBroadcastAddressFromInterfaces();
  QVector<QHostAddress> getLocalAddressFromInterfaces();

  bool isLocalHost(const QHostAddress&) const;

  QVector<QHostAddress> m_local_host;
  QVector<QHostAddress> m_broadcast_ip;
  QUdpSocket m_broadcast_udp;
  ReceiverModel* m_receiver;
  // QMap<QString, RemoteServer*> m_remote_servers;
};
