#include "transfer_server.h"

#include <QByteArray>
#include <QDataStream>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>

#include "config.h"
#include "receive_task.h"
#include "setting.h"

TransferServer::TransferServer(QObject* parent) : QTcpServer(parent) {
  m_pool = new QThreadPool(this);
  // How many threads I want at any given time
  // If there are more connections, they will be qued until a threads is closed
  m_pool->setMaxThreadCount(Setting::ins().m_max_send_thread);
  m_receive_buffer.clear();
}

TransferServer::~TransferServer() {}

void TransferServer::incomingConnection(qintptr socketDescriptor) {
  // ReceiveTask* receive_task = new ReceiveTask(socketDescriptor);
  // Delete that object when you're done (instead of using signals and slots)
  // receive_task->setAutoDelete(true);

  // m_pool->start(receive_task);
  qDebug() << "tcp handler: " << socketDescriptor;
  m_receive_socket = new QTcpSocket();
  m_receive_socket->setSocketDescriptor(socketDescriptor);
  connect(m_receive_socket, &QTcpSocket::readyRead, this,
          &TransferServer::onReadyRead);
}

void TransferServer::onReadyRead() {
  m_receive_buffer.append(m_receive_socket->readAll());

  while (m_receive_buffer.size()) {
    PackageSize size{0};
    memcpy(&size, m_receive_buffer.data(), sizeof(PackageSize));
    // m_receive_buffer.remove(0, sizeof(PackageSize));
    auto data = m_receive_buffer.mid(0, size);
    data.remove(0, sizeof(PackageSize));
    PackageType type;
    memcpy(&type, data.data(), sizeof(PackageType));
    data.remove(0, sizeof(PackageType));
    // Q_ASSERT(m_receive_buffer.size() == size);
    qDebug() << size;

    processPackage(type, data);
    m_receive_buffer.remove(0, size);
  }
}

void TransferServer::processPackage(PackageType type, QByteArray& data) {
  switch (type) {
    case PackageType::Header: {
      QJsonObject obj = QJsonDocument::fromJson(data).object();
      auto filename = QFileInfo(obj.value("name").toString());
      m_receive_file = new QFile(filename.fileName());
      m_receive_file->open(QIODevice::Append);

      qDebug() << filename;

      // m_receive_file->open()
    } break;
    case PackageType::Data: {
      m_receive_file->write(data);
    } break;
    case PackageType::Finish: {
      m_receive_file->close();
      m_receive_socket->disconnectFromHost();
    }

    default:
      break;
  }
}

void TransferServer::sendFile(const QString& filename,
                              const QHostAddress& dst) {
  // mInfo->setFilePath(mFilePath);
  auto sender = new SendTask(dst, filename);
  m_senders.push_back(sender);
  sender->run();
}

QByteArray TransferServer::preparePackage(PackageType type, QByteArray data) {
  QByteArray result;
  PackageSize size = sizeof(PackageSize) + sizeof(PackageType) + data.size();
  result.append((char*)(&size), sizeof(PackageSize));
  result.append((char*)(&type), sizeof(PackageType));
  result.append(data);
  return result;
}