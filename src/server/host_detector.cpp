#include "host_detector.h"

#include <QHostInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInterface>
#include <QProcess>
#include <QStandardPaths>

#include "config.h"
#include "dhcp_server.h"
#include "setting.h"

HostDetector::HostDetector(QObject *parent) : QObject(parent) {
  m_receiver_model = new ReceiverModel(this);
  connect(this, &HostDetector::addHost, m_receiver_model, &ReceiverModel::add);
  connect(this, &HostDetector::removeHost, m_receiver_model,
          &ReceiverModel::remove);

  m_local_host_ip = getLocalAddressFromInterfaces();
  m_broadcast_ip = getBroadcastAddressFromInterfaces();
// TODO: start dhcp if LAN no dhcp server
#if 0
    if (m_local_host_ip.empty()) {
#ifdef Q_OS_WIN
      dhcpServerStart();
#else
#endif
      m_local_host_ip = getLocalAddressFromInterfaces();
      m_broadcast_ip = getBroadcastAddressFromInterfaces();
    }
#endif

  qDebug() << "Local host";
  for (auto &&i : m_local_host_ip) {
    qDebug() << i.toString();
  }

  qDebug() << "Local broadcast";
  for (auto &&i : m_broadcast_ip) {
    qDebug() << i.toString();
  }

  // share port for broadcast and listen this port in same time
  m_broadcast_udp = new QUdpSocket(this);
  m_broadcast_udp->bind(DefaultBroadcastPort, QUdpSocket::ShareAddress);
  connect(m_broadcast_udp, &QUdpSocket::readyRead, this,
          &HostDetector::receiveBroadcast);
}

HostDetector::~HostDetector() {}

ReceiverModel *HostDetector::receiverModel() { return m_receiver_model; }

const QVector<QHostAddress> &HostDetector::hostIp() { return m_local_host_ip; }

void HostDetector::broadcast(MsgType type) {
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

QVector<QHostAddress> HostDetector::getBroadcastAddressFromInterfaces() {
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

void HostDetector::onUpdateSettings() { broadcast(MsgType::Update); }

void HostDetector::receiveBroadcast() {
  // receive broadcast info from other server
  // TODO: package stickness
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
      if (msg_type == MsgType::New || msg_type == MsgType::Update ||
          msg_type == MsgType::Reply) {
        emit addHost(remote_server);
      } else if (msg_type == MsgType::Delete) {
        emit removeHost(remote_server);
      }

      if (msg_type == MsgType::New) {
        // send back new remote server this host info
        sendHostInfo(sender, MsgType::Reply);
      }
    }
  }
}

void HostDetector::sendHostInfo(QHostAddress dst, MsgType t) {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BROAD_MSG_MAGIC},
                                               {"name", setting.m_hostname},
                                               {"os", OS_NAME},
                                               {"type", (int)t}}));

  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  m_broadcast_udp->writeDatagram(data, dst, port);
}

QVector<QHostAddress> HostDetector::getLocalAddressFromInterfaces() {
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

bool HostDetector::isLocalHost(const QHostAddress &addr) const {
  auto find = std::find(m_local_host_ip.begin(), m_local_host_ip.end(), addr);
  if (find != m_local_host_ip.end()) {
    return true;
  } else {
    return false;
  }
}