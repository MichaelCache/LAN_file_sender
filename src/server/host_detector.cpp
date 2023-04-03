#include "host_detector.h"

#include <QHostInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInterface>
#include <QProcess>
#include <QStandardPaths>

#include "config.h"
#include "setting.h"

HostDetector::HostDetector(QObject *parent) : QObject(parent) {
  // TODO:
  if (m_local_host_ip.empty()) {
// no LAN, establish DHCP
#ifdef Q_OS_WIN
    QString dhcp_command =
        "netsh dhcpserver add optiondef 60 \"PXEClient\" STRING 0 "
        "comment=\"PXE support\"";
#else
    QString dhcp_command = "sudo systemctl start isc-dhcp-server.service";
#endif
    QProcess dhcp_process;
    dhcp_process.start(dhcp_command);
    dhcp_process.waitForFinished();
    if (dhcp_process.exitCode() == 0) {
    } else {
      qDebug() << "DHCP server failed to start";
    }
  }

  m_receiver_model = new ReceiverModel(this);
  connect(this, &HostDetector::addHost, m_receiver_model, &ReceiverModel::add);
  connect(this, &HostDetector::removeHost, m_receiver_model,
          &ReceiverModel::remove);

  m_localhost_name = QHostInfo::localHostName();

  m_local_host_ip = getLocalAddressFromInterfaces();
  m_broadcast_ip = getBroadcastAddressFromInterfaces();

  // share port for broadcast and listen this port in same time
  m_broadcast_udp = new QUdpSocket(this);
  m_broadcast_udp->bind(DefaultBroadcastPort, QUdpSocket::ShareAddress);
  connect(m_broadcast_udp, &QUdpSocket::readyRead, this,
          &HostDetector::receiveBroadcast);
}

HostDetector::~HostDetector() {}

ReceiverModel *HostDetector::receiverModel() { return m_receiver_model; }

const QString &HostDetector::hostName() { return m_localhost_name; }
const QVector<QHostAddress> &HostDetector::hostIp() { return m_local_host_ip; }

void HostDetector::broadcast(MsgType type) {
  auto &setting = Setting::ins();
  int port = DefaultBroadcastPort;
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BroadCastMagic},
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
  QJsonObject obj(QJsonObject::fromVariantMap({{"magic", BroadCastMagic},
                                               {"name", setting.m_hostname},
                                               {"os", OS_NAME},
                                               {"type", (int)t}}));

  QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
  m_broadcast_udp->writeDatagram(data, dst, port);
}

QVector<QHostAddress> HostDetector::getLocalAddressFromInterfaces() {
  QVector<QHostAddress> addresses;
  for (auto &&iface : QNetworkInterface::allInterfaces()) {
    // ignore not running interface
    // ignore loop back interface
    if (!(iface.flags() & QNetworkInterface::IsRunning) ||
        (iface.flags() & QNetworkInterface::IsLoopBack)) {
      continue;
    }

    for (auto &&addressEntry : iface.addressEntries()) {
      auto ip = QHostAddress(addressEntry.ip().toIPv4Address());
      if ((ip == QHostAddress::Any) || (ip == QHostAddress::LocalHost) ||
          (ip == QHostAddress::LocalHostIPv6) || (ip == QHostAddress::Null) ||
          (ip == QHostAddress::AnyIPv4) || (ip == QHostAddress::AnyIPv6)) {
        continue;
      }
      addresses.push_back(ip);
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