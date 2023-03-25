#include "host_detector.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInterface>
#include <QStandardPaths>

#include "config.h"
#include "setting.h"
namespace {

QString msgTypeToStr(MsgType t) {
  switch (t) {
    case MsgType::New:
      return "New";
      break;
    case MsgType::Update:
      return "Update";
      break;
    case MsgType::Reply:
      return "Reply";
      break;
    case MsgType::Delete:
      return "Delete";
      break;
    default:
      return "None";
      break;
  }
}

MsgType strToMsgType(const QString &s) {
  if (s == "None") {
    return MsgType::None;
  } else if (s == "New") {
    return MsgType::New;
  } else if (s == "Update") {
    return MsgType::Update;
  } else if (s == "Reply") {
    return MsgType::Reply;
  } else if (s == "Delete") {
    return MsgType::Delete;
  } else {
  }
}

}  // namespace

HostDetector::HostDetector(QObject *parent) : QObject(parent) {
  m_local_host_ip = getLocalAddressFromInterfaces();
  m_broadcast_ip = getBroadcastAddressFromInterfaces();

  // share port for broadcast and listen this port in same time
  m_broadcast_udp = new QUdpSocket(this);
  m_broadcast_udp->bind(DefaultBroadcastPort, QUdpSocket::ShareAddress);
  connect(m_broadcast_udp, &QUdpSocket::readyRead, this,
          &HostDetector::receiveBroadcast);
}

HostDetector::~HostDetector() {}

void HostDetector::broadcast() {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(
      QJsonObject::fromVariantMap({{"magic", BroadCastMagic},
                                   {"name", setting.hostName()},
                                   {"os", OS_NAME},
                                   {"type", msgTypeToStr(MsgType::New)}}));

  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  for (auto &&address : m_broadcast_ip) {
    m_broadcast_udp->writeDatagram(data, address, port);
  }
}

QVector<QHostAddress> HostDetector::getBroadcastAddressFromInterfaces() {
  QVector<QHostAddress> addresses;
  for (auto &&iface : QNetworkInterface::allInterfaces()) {
    if ((iface.flags() & QNetworkInterface::CanBroadcast) &&
        !(iface.flags() & QNetworkInterface::IsLoopBack)) {
      for (auto &&addressEntry : iface.addressEntries()) {
        if (!addressEntry.broadcast().isNull()) {
          addresses.push_back(addressEntry.broadcast());
        }
      }
    }
  }
  return addresses;
}

void HostDetector::receiveBroadcast() {
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
    if (obj.value("magic").toString() == BroadCastMagic) {
      // filter out loopback and localhost ip
      if (sender.isLoopback() || isLocalHost(sender)) {
        continue;
      }

      RemoteHostInfo remote_server{sender, obj.value("name").toString(),
                                   obj.value("os").toString()};

      emit addHost(remote_server);
      auto msg_type = strToMsgType(obj.value("type").toString());
      if (msg_type == MsgType::New) {
        // tell new remote server this host info
        sendHostInfo(sender, MsgType::Reply);
      } else if (msg_type == MsgType::Delete) {
        emit removeHost(remote_server);
      }
    }
  }
}

void HostDetector::sendHostInfo(QHostAddress dst, MsgType t) {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BroadCastMagic},
                                               {"name", setting.hostName()},
                                               {"os", OS_NAME},
                                               {"type", msgTypeToStr(t)}}));

  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  m_broadcast_udp->writeDatagram(data, dst, port);
}

QVector<QHostAddress> HostDetector::getLocalAddressFromInterfaces() {
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

bool HostDetector::isLocalHost(const QHostAddress &addr) const {
  auto find = std::find(m_local_host_ip.begin(), m_local_host_ip.end(), addr);
  if (find != m_local_host_ip.end()) {
    return true;
  } else {
    return false;
  }
}