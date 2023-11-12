#pragma once

#include <QAtomicInt>
#include <QHostAddress>
#include <QMutex>
#include <QSet>
#include <QSignalMapper>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QUdpSocket>

#include "model/host_info.h"
#include "model/host_model.h"

enum class MsgType : int { None = 0, New, Update, Reply, Delete };

class HostBroadcaster : public QObject {
  Q_OBJECT
 public:
  HostBroadcaster(QObject* parent = nullptr);
  ~HostBroadcaster() = default;

  void start();
  void stop();

  const QVector<QHostAddress>& hostIp();
  void broadcast(MsgType type);

 Q_SIGNALS:
  void detectHostOnLine(const RemoteHostInfo&);
  void detectHostOffline(const RemoteHostInfo&);

 public Q_SLOTS:
  void onUpdateHostInfo();

 private Q_SLOTS:
  void consistBroadcast();
  void receiveBroadcast();

 private:
  void sendHostInfo(QHostAddress, MsgType);
  QVector<QHostAddress> getBroadcastAddressFromInterfaces();
  QVector<QHostAddress> getLocalAddressFromInterfaces();

  bool isLocalHost(const QHostAddress&) const;

  // ReceiverModel* m_receiver_model;

  QVector<QHostAddress> m_local_host_ip;
  QVector<QHostAddress> m_broadcast_ip;
  QUdpSocket* m_broadcast_udp;
  QTimer* m_timer;
  QSet<QHostAddress> m_added_host;
};
