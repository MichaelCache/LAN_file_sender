#include "control_server.h"

#include <QDataStream>
#include <QDir>
#include <QFileInfo>
#include <QTcpSocket>

#include "setting.h"

void ControlServer::start() {
  listen(QHostAddress::Any, Setting::ins().m_file_info_port);
}

void ControlServer::stop() {
  // do nothing
}

void ControlServer::sendFileInfo(const QHostAddress& dst_ip,
                                 QVector<TransferInfo> info,
                                 ControlSignal signal, qint32 send_port) {
  for (auto&& i : info) {
    auto address = dst_ip;
    // use FileInfo to send tcp package for save network load
    QVector<FileInfo> file_infos;
    file_infos.push_back(FileInfo{i.m_file_name, i.m_file_size, i.id()});
    QTcpSocket* info_socket = nullptr;
    if (m_info_sender.contains(address)) {
      // socket had created, just reuse it
      info_socket = m_info_sender.value(address);
      auto state = info_socket->state();
      if (info_socket->state() != QAbstractSocket::ConnectedState) {
        // try reconnecte
        info_socket->connectToHost(address, send_port);
      }
    } else {
      // create new socket
      info_socket = new QTcpSocket(this);
      m_info_sender.insert(address, info_socket);
      info_socket->connectToHost(address, send_port);
    }
    auto buffer = this->packFileInfoPackage(signal, file_infos);
    info_socket->write(buffer);
  }
}

void ControlServer::incomingConnection(descriptor descriptor) {
  // try clear disconnected socket
  clearDisconnectedReciver();
  // create new socket to recieve package
  auto info_socket = new QTcpSocket(this);
  m_info_reciever.insert(descriptor, info_socket);
  connect(
      info_socket, &QTcpSocket::readyRead, [info_socket, descriptor, this]() {
        QByteArray& data = this->m_info_recieve_cache[descriptor];
        data.append(info_socket->readAll());
        auto&& infos = this->unpackFileInfoPackage(data);
        auto& signal = std::get<0>(infos);
        auto& files = std::get<1>(infos);
        auto from_ip = QHostAddress(info_socket->peerAddress().toIPv4Address());

        QVector<TransferInfo> trans_infos;
        for (auto&& i : files) {
          TransferInfo trans_info(i);
          trans_info.m_file_path = QDir(Setting::ins().m_download_dir)
                                       .filePath(trans_info.m_file_name);
          trans_info.m_from_ip = from_ip;
          trans_info.m_state = TransferState::Pending;
          trans_infos.push_back(trans_info);
        }

        switch (signal) {
          case ControlSignal::InfoSend:
            emit this->recieveFileInfo(trans_infos);
            break;
          case ControlSignal::CancelSend:
            emit this->sendFileBeCancelled(trans_infos);
            break;
          case ControlSignal::AcceptSend:
            emit this->remoteAccept(trans_infos);
            break;
          case ControlSignal::RejectSend: {
            for (auto&& i : trans_infos) {
              std::swap(i.m_dest_ip, i.m_from_ip);
              i.m_state = TransferState::Rejected;
            }

            emit this->remoteReject(trans_infos);
            break;
          }
          default:
            break;
        }
      });
  info_socket->setSocketDescriptor(descriptor);
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

void ControlServer::clearDisconnectedReciver() {
  QVector<qint32> remove_cache;
  for (auto&& k : m_info_reciever.keys()) {
    auto r = m_info_reciever.value(k);
    if (r->state() != QAbstractSocket::ConnectedState) {
      remove_cache.push_back(k);
    }
  }

  for (auto&& s : remove_cache) {
    m_info_reciever.remove(s);
    m_info_recieve_cache.remove(s);
  }
}
