#include "transfer_server.h"

#include <QByteArray>
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
  m_pool->setMaxThreadCount(Setting::ins().m_max_thread);
}

TransferServer::~TransferServer() {}

void TransferServer::incomingConnection(qintptr socketDescriptor) {
  // ReceiveTask* receive_task = new ReceiveTask(socketDescriptor);
  // Delete that object when you're done (instead of using signals and slots)
  // receive_task->setAutoDelete(true);

  // m_pool->start(receive_task);
  qDebug() << "tcp handler: " << socketDescriptor;
  auto receive_socket = new QTcpSocket();
  receive_socket->setSocketDescriptor(socketDescriptor);
  auto data = receive_socket->readAll();
  PackageSize size;
  memcpy(&size, data.data(), sizeof(PackageSize));
  data.remove(0, sizeof(PackageSize));
  PackageType type;
  memcpy(&type, data.data(), sizeof(PackageType));
  data.remove(0, sizeof(PackageType));
  QJsonObject obj = QJsonDocument::fromJson(data).object();
  qDebug() << size << " "  << " " << obj;
}

void TransferServer::sendFile(const QString& filename,
                              const QHostAddress& dst) {
  // mInfo->setFilePath(mFilePath);
  m_send_file = new QFile(filename, this);
  bool ok = m_send_file->open(QIODevice::ReadOnly);
  qint64 file_size;
  if (ok) {
    file_size = m_send_file->size();
    // mInfo->setDataSize(mFileSize);
    // mBytesRemaining = mFileSize;
    // emit mInfo->fileOpened();
  }

  if (file_size > 0) {
    m_send_socket = new QTcpSocket(this);
    m_send_socket->connectToHost(dst, DefaultTransferPort,
                                 QAbstractSocket::ReadWrite);
    // mInfo->setState(TransferState::Waiting);

    connect(m_send_socket, &QTcpSocket::bytesWritten, this,
            &TransferServer::onBytesWritten);
    connect(m_send_socket, &QTcpSocket::connected, this,
            &TransferServer::onConnected);
    connect(m_send_socket, &QTcpSocket::disconnected, this,
            &TransferServer::onDisconnected);
  }

  //   return ok && mSocket;
}

void TransferServer::onBytesWritten(qint64 bytes) {
  Q_UNUSED(bytes);

  if (!m_send_file->bytesToWrite()) {
    // sendData();
  }
}

void TransferServer::onConnected() {
  QString filename = m_send_file->fileName();
  auto filesize = m_send_file->size();

  QJsonObject obj(
      QJsonObject::fromVariantMap({{"name", filename}, {"size", filesize}}));

  QByteArray header_data(QJsonDocument(obj).toJson());

  QByteArray send_data = preparePackage(PackageType::Header, header_data);
  m_send_socket->write(send_data);
}
void TransferServer::onDisconnected() {}

QByteArray TransferServer::preparePackage(PackageType type, QByteArray data) {
  QByteArray result;
  PackageSize size = sizeof(PackageSize) + sizeof(PackageType) + data.size();
  result.append((char*)(&size), sizeof(PackageSize));
  result.append((char*)(&type), sizeof(PackageType));
  result.prepend(data);
  return result;
}