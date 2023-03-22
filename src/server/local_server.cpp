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
  m_receiver = new ReceiverModel(this);
  m_local_host = getLocalAddressFromInterfaces();
  m_broadcast_ip = getBroadcastAddressFromInterfaces();
}

LocalServer::~LocalServer() {}

ReceiverModel *LocalServer::receivers() { return m_receiver; }

void LocalServer::sendBroadcast() {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BroadCastMagic},
                                               {"name", setting.hostName()},
                                               {"os", OS_NAME},
                                               {"type", "broadcast"}}));

  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  for (auto &&address : m_broadcast_ip) {
    m_broadcast_udp.writeDatagram(data, address, port);
  }
}

QVector<QHostAddress> LocalServer::getBroadcastAddressFromInterfaces() {
  QVector<QHostAddress> addresses;
  for (auto &&iface : QNetworkInterface::allInterfaces()) {
    if ((iface.flags() & QNetworkInterface::CanBroadcast) &&
        !(iface.flags() & QNetworkInterface::IsLoopBack)) {
      for (auto &&addressEntry : iface.addressEntries()) {
        if (!addressEntry.broadcast().isNull()) {
          addresses.push_back(addressEntry.broadcast());
          // qDebug() << addressEntry.broadcast().toString();
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
    quint16 port;

    m_broadcast_udp.readDatagram(data.data(), data.size(), &sender, &port);

    // convert to ipv4 if work in ipv4/v6 hybird
    sender = QHostAddress(sender.toIPv4Address());

    QJsonObject obj = QJsonDocument::fromJson(data).object();
    // if () {
    if (obj.value("magic").toString() == BroadCastMagic) {
      if (sender.isLoopback() || isLocalHost(sender)) {
        continue;
      }

      RemoteServer remote_server{sender, obj.value("name").toString(),
                                 obj.value("os").toString()};

      if (m_receiver->add(remote_server) &&
          obj.value("type").toString() != "replay") {
        // m_remote_servers.insert(sender.toString(), remote_server);
        // tell new remote server self host info
        sendHostInfo(sender, "reply");
      }
    }
    // }
  }
}

void LocalServer::sendHostInfo(QHostAddress dst, const QString &type) {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BroadCastMagic},
                                               {"name", setting.hostName()},
                                               {"os", OS_NAME},
                                               {"type", type}}));

  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  m_broadcast_udp.writeDatagram(data, dst, port);
}

QVector<QHostAddress> LocalServer::getLocalAddressFromInterfaces() {
  QVector<QHostAddress> addresses;
  for (auto &&iface : QNetworkInterface::allInterfaces()) {
    if (!(iface.flags() & QNetworkInterface::IsLoopBack)) {
      for (auto &&addressEntry : iface.addressEntries()) {
        addresses.push_back(addressEntry.ip());
      }
    }
  }
  return addresses;
}

bool LocalServer::isLocalHost(const QHostAddress &addr) const {
  auto find = std::find(m_local_host.begin(), m_local_host.end(), addr);
  if (find != m_local_host.end()) {
    return true;
  } else {
    return false;
  }
}