#include "host_broadcaster.h"

#include <QHostInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInterface>
#include <QProcess>
#include <QStandardPaths>

#include "config.h"
#include "setting.h"

HostBroadcaster::HostBroadcaster(QObject *parent) : QObject(parent) {
  m_local_host_ip = getLocalAddressFromInterfaces();
  m_broadcast_ip = getBroadcastAddressFromInterfaces();

  m_timer = new QTimer(this);

  // share port for broadcast and listen this port in same time
  m_broadcast_udp = new QUdpSocket(this);
  m_broadcast_udp->bind(DefaultBroadcastPort, QUdpSocket::ShareAddress);
  connect(m_broadcast_udp, &QUdpSocket::readyRead, this,
          &HostBroadcaster::receiveBroadcast);
  connect(m_timer, &QTimer::timeout, this, &HostBroadcaster::consistBroadcast);
  connect(&Setting::ins(), &Setting::hostnameChanged, this,
          &HostBroadcaster::onUpdateHostInfo);
  broadcast(MsgType::New);
}

void HostBroadcaster::start() {
  broadcast(MsgType::New);
  m_timer->start(Setting::ins().m_boradcast_interval);
}

const QVector<QHostAddress> &HostBroadcaster::hostIp() {
  return m_local_host_ip;
}

void HostBroadcaster::broadcast(MsgType type) {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BROAD_MSG_MAGIC},
                                               {"name", setting.m_hostname},
                                               {"os", OS_NAME},
                                               {"type", (int)type}}));

  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  for (auto &&address : m_broadcast_ip) {
    m_broadcast_udp->writeDatagram(data, address, port);
  }
}

QVector<QHostAddress> HostBroadcaster::getBroadcastAddressFromInterfaces() {
  QVector<QHostAddress> addresses;
  for (auto &&iface : QNetworkInterface::allInterfaces()) {
    auto flag = iface.flags();
    if ((flag & QNetworkInterface::CanBroadcast) &&
        !(flag & QNetworkInterface::IsLoopBack) &&
        (flag & QNetworkInterface::IsRunning)) {
      for (auto &&addressEntry : iface.addressEntries()) {
        if (!addressEntry.broadcast().isNull()) {
          addresses.push_back(addressEntry.broadcast());
        }
      }
    }
  }
  return addresses;
}

void HostBroadcaster::onUpdateHostInfo() { broadcast(MsgType::Update); }

void HostBroadcaster::stop() { broadcast(MsgType::Delete); }

void HostBroadcaster::consistBroadcast() { broadcast(MsgType::New); }

void HostBroadcaster::receiveBroadcast() {
  // receive broadcast info from other server
  while (m_broadcast_udp->hasPendingDatagrams()) {
    QByteArray data;

    qint64 datagram_size = m_broadcast_udp->pendingDatagramSize();
    assert(datagram_size <= std::numeric_limits<int>::max());

    data.resize(static_cast<int>(datagram_size));
    QHostAddress sender;
    quint16 port;

    m_broadcast_udp->readDatagram(data.data(), data.size(), &sender, &port);

    // convert to ipv4 if work in ipv4/v6 hybird interface
    sender = QHostAddress(sender.toIPv4Address());

    QJsonObject obj = QJsonDocument::fromJson(data).object();
    if (obj.value("magic").toString() == BROAD_MSG_MAGIC) {
      // filter out loopback and localhost ip
      if (sender.isLoopback() || isLocalHost(sender)) {
        continue;
      }

      RemoteHostInfo remote_server{sender, obj.value("name").toString(),
                                   obj.value("os").toString()};

      auto msg_type = (MsgType)(obj.value("type").toInt());
      if (msg_type == MsgType::New) {
        if (!m_added_host.contains(sender)) {
          emit detectHostOnLine(remote_server);
        }

      } else if (msg_type == MsgType::Update || msg_type == MsgType::Reply) {
        emit detectHostOnLine(remote_server);
      } else if (msg_type == MsgType::Delete) {
        emit detectHostOffline(remote_server);
        m_added_host.remove(sender);
      }

      if (msg_type == MsgType::New && !m_added_host.contains(sender)) {
        // send back new remote server this host info
        sendHostInfo(sender, MsgType::Reply);
        m_added_host.insert(sender);
      }
    }
  }
}

void HostBroadcaster::sendHostInfo(QHostAddress dst, MsgType t) {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BROAD_MSG_MAGIC},
                                               {"name", setting.m_hostname},
                                               {"os", OS_NAME},
                                               {"type", (int)t}}));

  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  m_broadcast_udp->writeDatagram(data, dst, port);
}

QVector<QHostAddress> HostBroadcaster::getLocalAddressFromInterfaces() {
  QVector<QHostAddress> addresses;
  for (auto &&iface : QNetworkInterface::allInterfaces()) {
    auto flag = iface.flags();
    // ignore loop back interface and ignore not up interface
    if (!(flag & QNetworkInterface::IsLoopBack) &&
        (flag & QNetworkInterface::IsUp)) {
      for (auto &&addressEntry : iface.addressEntries()) {
        auto ip = QHostAddress(addressEntry.ip().toIPv4Address());
        if ((ip == QHostAddress::Any) ||
            // ignore 127.0.0.1
            (ip == QHostAddress::LocalHost) ||
            (ip == QHostAddress::LocalHostIPv6) || (ip == QHostAddress::Null) ||
            (ip == QHostAddress::AnyIPv4) || (ip == QHostAddress::AnyIPv6)) {
          continue;
        }
        addresses.push_back(ip);
      }
    }
  }
  return addresses;
}

bool HostBroadcaster::isLocalHost(const QHostAddress &addr) const {
  auto find = std::find(m_local_host_ip.begin(), m_local_host_ip.end(), addr);
  if (find != m_local_host_ip.end()) {
    return true;
  } else {
    return false;
  }
}