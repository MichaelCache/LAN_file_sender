#pragma once

#include <QAtomicInt>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QUdpSocket>

#include "model/host_info.h"
#include "model/receiver_model.h"

enum class MsgType : int { None = 0, New, Update, Reply, Delete };

class HostDetector : public QObject {
  Q_OBJECT
 public:
  HostDetector(QObject* parent = nullptr);
  ~HostDetector();

  ReceiverModel* receiverModel();

 Q_SIGNALS:
  void addHost(const RemoteHostInfo&);
  void removeHost(const RemoteHostInfo&);

 public Q_SLOTS:
  void broadcast(MsgType type);

 private Q_SLOTS:
  void receiveBroadcast();

 private:
  void sendHostInfo(QHostAddress, MsgType);
  QVector<QHostAddress> getBroadcastAddressFromInterfaces();
  QVector<QHostAddress> getLocalAddressFromInterfaces();

  bool isLocalHost(const QHostAddress&) const;

  ReceiverModel* m_receiver_model;

  QVector<QHostAddress> m_local_host_ip;
  QVector<QHostAddress> m_broadcast_ip;
  QString m_localhost_name;
  QUdpSocket* m_broadcast_udp;
};
