#include "control_server.h"

#include <QDataStream>
#include <QFileInfo>
#include <QTcpSocket>

#include "setting.h"

ControlServer::ControlServer(QObject* parent, quint32 file_info_port)
    : QTcpServer(parent), m_file_info_port(file_info_port) {
  listen(QHostAddress::Any, m_file_info_port);
}

ControlServer::~ControlServer() {}

void ControlServer::sendFileInfo(const QVector<FileInfo>& info,
                                 const QHostAddress& dst) {
  send(info, dst, ControlSignal::InfoSend);
}

void ControlServer::cancelFileInfo(const QVector<FileInfo>& info,
                                   const QHostAddress& dst) {
  send(info, dst, ControlSignal::CancelSend);
}

void ControlServer::acceptFileInfo(const QVector<FileInfo>& info,
                                   const QHostAddress& dst) {
  send(info, dst, ControlSignal::AcceptSend);
}

void ControlServer::denyFileInfo(const QVector<FileInfo>& info,
                                 const QHostAddress& dst) {
  send(info, dst, ControlSignal::DenySend);
}

void ControlServer::incomingConnection(qintptr descriptor) {
  if (m_info_reciever.contains(descriptor)) {
    auto info_socket = m_info_reciever.value(descriptor);
    if (info_socket->state() != QAbstractSocket::ConnectedState) {
      info_socket->setSocketDescriptor(descriptor);
    }
  } else {
    // create new socket to recieve package
    auto info_socket = new QTcpSocket();
    m_info_reciever.insert(descriptor, info_socket);
    connect(info_socket, &QTcpSocket::readyRead,
            [info_socket, descriptor, this]() {
              QByteArray& data = this->m_info_recieve_cache[descriptor];
              data.append(info_socket->readAll());
              auto&& infos = this->unpackFileInfoPackage(data);
              auto& signal = std::get<0>(infos);
              auto& files = std::get<1>(infos);
              auto from_ip =
                  QHostAddress(info_socket->peerAddress().toIPv4Address());
              switch (signal) {
                case ControlSignal::InfoSend:
                  emit this->recieveFile(files, from_ip);
                  break;
                case ControlSignal::CancelSend:
                  emit this->cancelFile(files, from_ip);
                case ControlSignal::AcceptSend:
                  emit this->acceptFile(files, from_ip);
                case ControlSignal::DenySend:
                  emit this->denyFile(files, from_ip);
                default:
                  break;
              }
            });
    info_socket->setSocketDescriptor(descriptor);
  }
}

QByteArray ControlServer::packFileInfoPackage(ControlSignal control,
                                              const QVector<FileInfo>& info) {
  QByteArray data_buffer;
  int pack_len = 0;
  QDataStream stream(&data_buffer, QIODevice::WriteOnly);
  // tcp message format is : package size, control signal, fileinfos
  stream << pack_len << (int)control << info;
  pack_len = data_buffer.size();
  // rewrite package size at first 4 byte;
  stream.device()->seek(0);
  stream << pack_len;
  return data_buffer;
}

std::tuple<ControlSignal, QVector<FileInfo>>
ControlServer::unpackFileInfoPackage(QByteArray& data) {
  QDataStream stream(data);
  int size = 0;
  stream >> size;
  // tcp package is sliced, wait for more data
  if (data.size() < size) {
    return {};
  }

  int sig = 0;
  stream >> sig;
  ControlSignal signal = (ControlSignal)sig;
  QVector<FileInfo> files;
  stream >> files;
  data.remove(0, size);
  return std::tuple<ControlSignal, QVector<FileInfo>>{signal, files};
}

void ControlServer::send(const QVector<FileInfo>& info,
                         const QHostAddress& address,
                         const ControlSignal& signal) {
  if (m_info_sender.contains(address)) {
    // socket had created
    auto info_socket = m_info_sender.value(address);
    if (info_socket->state() != QAbstractSocket::ConnectedState) {
      // if not disconnected, try reconnecte
      info_socket->connectToHost(address, m_file_info_port);
    }
    auto buffer = this->packFileInfoPackage(signal, info);
    info_socket->write(buffer);
  } else {
    // create new socket
    auto info_socket = new QTcpSocket(this);
    m_info_sender.insert(address, info_socket);
    connect(info_socket, &QTcpSocket::connected,
            [info_socket, signal, info, this]() {
              auto buffer = this->packFileInfoPackage(signal, info);
              info_socket->write(buffer);
            });
    info_socket->connectToHost(address, m_file_info_port);
  }
}
