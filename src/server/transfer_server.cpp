#include "transfer_server.h"

#include <QByteArray>
#include <QDataStream>
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
    m_receive_buffer.remove(0, sizeof(PackageSize));

    PackageType type;
    memcpy(&type, m_receive_buffer.data(), sizeof(PackageType));
    m_receive_buffer.remove(0, sizeof(PackageType));
    // Q_ASSERT(m_receive_buffer.size() == size);
    qDebug() << size;
    processPackage(type, m_receive_buffer);
    m_receive_buffer.remove(0, size);
  }
}

void TransferServer::processPackage(PackageType type, QByteArray& data) {
  switch (type) {
    case PackageType::Header: {
      QJsonObject obj = QJsonDocument::fromJson(data).object();
      auto filename = obj.value("name").toString();
      qDebug() << filename;

      // m_receive_file->open()
    } break;

    default:
      break;
  }
}

void TransferServer::sendFile(const QString& filename,
                              const QHostAddress& dst) {
  // mInfo->setFilePath(mFilePath);
  m_send_file = new QFile(filename, this);
  bool ok = m_send_file->open(QIODevice::ReadOnly);
  if (ok) {
    m_file_size = m_send_file->size();
    m_byte_remain = m_file_size;
    // mInfo->setDataSize(mFileSize);
    // mBytesRemaining = mFileSize;
    // emit mInfo->fileOpened();
  }

  if (m_file_size > 0) {
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
  // send file data
  if (!m_send_file->bytesToWrite()) {
    QByteArray file_buffer(DefaultFileBufferSize);
    // if (m_byte_remain < m_file_size) {
    //   mFileBuff.resize(mBytesRemaining);
    //   mFileBuffSize = mFileBuff.size();
    // }

    qint64 bytes_read =
        m_send_file->read(file_buffer.data(), DefaultFileBufferSize);
    if (bytes_read == -1) {
      // emit mInfo->errorOcurred(tr("Error while reading file."));
      return;
    }

    m_byte_remain -= bytes_read;
    if (m_byte_remain < 0) m_byte_remain = 0;

    // mInfo->setProgress((int)((mFileSize - mBytesRemaining) * 100 /
    // mFileSize));
    auto data = preparePackage(PackageType::Data, file_buffer);
    m_send_socket->write(data);
    // writePacket(mFileBuffSize, PacketType::Data, mFileBuff);

    if (!m_byte_remain) {
      finish();
    }
  }
}

void TransferServer::finish() {
  m_send_file->close();
  // mInfo->setState(TransferState::Finish);
  // emit mInfo->done();
  auto data = preparePackage(PackageType::Finish);
  m_send_socket->write(data);
}

void TransferServer::onConnected() {
  // send file header
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
  PackageSize size = data.size();
  result.append((char*)(&size), sizeof(PackageSize));
  result.append((char*)(&type), sizeof(PackageType));
  result.prepend(data);
  return result;
}