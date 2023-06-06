#include "control_server.h"

#include <QDataStream>
#include <QFileInfo>
#include <QTcpSocket>

#include "control_state.h"
#include "setting.h"

ControlServer::ControlServer(QObject* parent) : QTcpServer(parent) {
  m_transfer_server = new TransferServer(parent);
}

ControlServer::~ControlServer() {}

void ControlServer::onSendFile(const QStringList& filenames,
                               const QHostAddress& dst) {
  QTcpSocket* info_socket = getSender(dst);
  auto buffer = packFileInfoPackage(ControlSignal::SendInfo, filenames);
  info_socket->write(buffer);
}

void ControlServer::onCancelSend(const QStringList& filenames,
                                 const QHostAddress& dst) {
  QTcpSocket* info_socket = getSender(dst);
  auto buffer = packFileInfoPackage(ControlSignal::CancelSend, filenames);
  info_socket->write(buffer);
}

void ControlServer::incomingConnection(qintptr socket_descriptor) {
  auto info_socket = getReciever(socket_descriptor);
  connect(info_socket, &QTcpSocket::readyRead,
          [&info_socket, &socket_descriptor, this]() {
            QByteArray& data = this->m_info_recieve_cache[socket_descriptor];
            data.append(info_socket->readAll());
            auto&& infos = this->unpackFileInfoPackage(data);
            auto& signal = std::get<0>(infos);
            auto& files = std::get<1>(infos);
            auto from_ip =
                QHostAddress(info_socket->peerAddress().toIPv4Address());
            switch (signal) {
              case ControlSignal::SendInfo:
                emit this->recieveFileInfo(files, from_ip);
                break;
              case ControlSignal::CancelSend:
                emit this->cancelFileInfo(files, from_ip);
              default:
                break;
            }
          });
}

QByteArray&& ControlServer::packFileInfoPackage(ControlSignal control,
                                                const QStringList& filenames) {
  QByteArray buffer;
  QDataStream stream(&buffer, QIODevice::WriteOnly);
  stream << (int)control << fileListToFileInfo(filenames);
  auto pack_len = buffer.size();
  buffer.insert(0, (char*)&pack_len, (int)sizeof(pack_len));
  return std::move(buffer);
}

std::tuple<ControlSignal, QVector<FileInfo>>&&
ControlServer::unpackFileInfoPackage(const QByteArray& data) {
  QDataStream stream(data);
  int size = 0;
  stream >> size;
  if (data.size() < size) {
    return std::move(std::tuple<ControlSignal, QVector<FileInfo>>{});
  }

  int sig = 0;
  stream >> sig;
  ControlSignal signal = (ControlSignal)sig;
  QVector<FileInfo> files;
  stream >> files;
  return std::move(std::tuple<ControlSignal, QVector<FileInfo>>{signal, files});
}

QVector<FileInfo>&& ControlServer::fileListToFileInfo(
    const QStringList& filenames) {
  QVector<FileInfo> infos;
  for (auto&& i : filenames) {
    QFileInfo fileinfo(i);
    FileInfo info{i, fileinfo.size()};
    infos.push_back(info);
  }
  return std::move(infos);
}

QTcpSocket* ControlServer::getSender(const QHostAddress& address) {
  QTcpSocket* info_socket;
  if (m_info_sender.contains(address)) {
    info_socket = m_info_sender.value(address);
  } else {
    info_socket = new QTcpSocket(this);
    info_socket->connectToHost(address, Setting::ins().m_file_info_port);
    m_info_sender.insert(address, info_socket);
    connect(info_socket, &QTcpSocket::disconnected, info_socket,
            &QTcpServer::deleteLater);
    connect(info_socket, &QTcpSocket::disconnected, info_socket,
            [&address, this]() { this->m_info_sender.remove(address); });
  }
  return info_socket;
}

QTcpSocket* ControlServer::getReciever(const qintptr& descriptor) {
  QTcpSocket* info_socket;
  if (m_info_reciever.contains(descriptor)) {
    info_socket = m_info_reciever.value(descriptor);
  } else {
    info_socket = new QTcpSocket(this);
    info_socket->setSocketDescriptor(descriptor);
    m_info_reciever.insert(descriptor, info_socket);
    connect(info_socket, &QTcpSocket::disconnected, info_socket,
            &QTcpServer::deleteLater);
    connect(
        info_socket, &QTcpSocket::disconnected, info_socket,
        [&descriptor, this]() { this->m_info_reciever.remove(descriptor); });
  }
  return info_socket;
}