#pragma once

#include <QAtomicInt>
#include <QHostAddress>
#include <QMutex>
#include <QSignalMapper>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSet>
#include <QUdpSocket>
#include <QTimer>

#include "model/host_info.h"
#include "model/receiver_model.h"

enum class MsgType : int { None = 0, New, Update, Reply, Delete };

class HostBroadcaster : public QObject {
  Q_OBJECT
 public:
  HostBroadcaster(QObject* parent = nullptr);
  ~HostBroadcaster();

  ReceiverModel* receiverModel();

  const QVector<QHostAddress>& hostIp();
  void broadcast(MsgType type);

 Q_SIGNALS:
  void detectNewHost(const RemoteHostInfo&);
  void detectHostOffline(const RemoteHostInfo&);

 public Q_SLOTS:
  void onUpdateHostInfo();
  void stop();

 private Q_SLOTS:
  void consistBroadcast();
  void receiveBroadcast();

 private:
  void sendHostInfo(QHostAddress, MsgType);
  QVector<QHostAddress> getBroadcastAddressFromInterfaces();
  QVector<QHostAddress> getLocalAddressFromInterfaces();

  bool isLocalHost(const QHostAddress&) const;

  ReceiverModel* m_receiver_model;

  QVector<QHostAddress> m_local_host_ip;
  QVector<QHostAddress> m_broadcast_ip;
  QUdpSocket* m_broadcast_udp;
  QTimer* m_timer;
  QSet<QHostAddress> m_added_host;
};
