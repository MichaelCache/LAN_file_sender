#include "local_server.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInterface>
#include <QStandardPaths>

#include "config.h"
#include "setting.h"

LocalServer::LocalServer(QObject *parent) : QLocalServer(parent) {
  // share port for broadcast and listen this port in same time
  m_broadcast_udp.bind(DefaultBroadcastPort, QUdpSocket::ShareAddress);
  connect(&m_broadcast_udp, &QUdpSocket::readyRead, this,
          &LocalServer::receiveBroadcast);

  // sendBroadcast();
}

LocalServer::~LocalServer() {}

void LocalServer::sendBroadcast() {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BroadCastMagic},
                                               {"name", setting.hostName()},
                                               {"os", OS_NAME}}));

  QVector<QHostAddress> addresses = getBroadcastAddressFromInterfaces();
  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  for (auto &&address : addresses) {
    m_broadcast_udp.writeDatagram(data, address, port);
  }
}

QVector<QHostAddress> LocalServer::getBroadcastAddressFromInterfaces() {
  QVector<QHostAddress> addresses;
  for (auto &&iface : QNetworkInterface::allInterfaces()) {
    if (iface.flags() & QNetworkInterface::CanBroadcast) {
      for (auto &&addressEntry : iface.addressEntries()) {
        if (!addressEntry.broadcast().isNull()) {
          addresses.push_back(addressEntry.broadcast());
        }
      }
    }
  }
  return addresses;
}

void LocalServer::receiveBroadcast() {
  // receive broadcast info from other server
  while (m_broadcast_udp.hasPendingDatagrams()) {
    QByteArray data;

    qint64 datagram_size = m_broadcast_udp.pendingDatagramSize();
    assert(datagram_size <= std::numeric_limits<int>::max());

    data.resize(static_cast<int>(datagram_size));
    QHostAddress sender;

    m_broadcast_udp.readDatagram(data.data(), data.size(), &sender);

    QJsonObject obj = QJsonDocument::fromJson(data).object();
    if (obj.keys().length() == 4) {
      if (obj.value("magic").toString() == BroadCastMagic) {
        if (m_remote_servers.contains(sender.toString())) {
          continue;
        }
        RemoteServer remote_server{sender, obj.value("name").toString(),
                                   obj.value("os").toString()};

        m_remote_servers.insert(sender.toString(), remote_server);
        // tell new remote server self host info
        sendHostInfo(sender);
      }
    }
  }
}

void LocalServer::sendHostInfo(QHostAddress dst) {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BroadCastMagic},
                                               {"name", setting.hostName()},
                                               {"os", OS_NAME}}));

  QVector<QHostAddress> addresses = getBroadcastAddressFromInterfaces();
  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  m_broadcast_udp.writeDatagram(data, dst, port);
}