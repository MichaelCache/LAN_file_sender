#include "receive_task.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include "package_type.h"
#include "setting.h"

ReceiveTask::ReceiveTask(qintptr descriptor, QObject* parent)
    : QThread(parent), m_socket_descriptor(descriptor) {}

ReceiveTask::~ReceiveTask() {}

void ReceiveTask::run() {
  m_socket = new QTcpSocket(this);
  if (m_socket->setSocketDescriptor(m_socket_descriptor)) {
    connect(m_socket, &QTcpSocket::readyRead, this, &ReceiveTask::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this,
            &ReceiveTask::onDisconnected);
  }
}

void ReceiveTask::onReadyRead() {
  m_buff.append(m_socket->readAll());
  while (m_buff.size()) {
    PackageSize size{0};
    memcpy(&size, m_buff.data(), sizeof(PackageSize));

    if (size > m_buff.size()) {
      // buff not cotains completely package, wait for more data
      break;
    }

    auto data = m_buff.mid(0, size);
    data.remove(0, sizeof(PackageSize));
    PackageType type;
    memcpy(&type, data.data(), sizeof(PackageType));
    data.remove(0, sizeof(PackageType));

    processPackage(type, data);
    m_buff.remove(0, size);
  }
}

void ReceiveTask::onDisconnected() {
  m_file->close();
  quit();
}

void ReceiveTask::processPackage(PackageType type, QByteArray& data) {
  switch (type) {
    case PackageType::Header:
      processPackageHeader(data);
      break;
    case PackageType::Data:
      processPackageData(data);
      break;
    case PackageType::Finish:
      processPackageFinish(data);
      m_socket->disconnectFromHost();
      break;
    default:
      break;
  }
}

void ReceiveTask::processPackageHeader(QByteArray& data) {
  QJsonObject obj = QJsonDocument::fromJson(data).object();
  m_filename = obj.value("name").toString();
  m_file = new QFile(m_filename);
  m_file->open(QIODevice::Append);
  m_from = QHostAddress(m_socket->peerAddress().toIPv4Address());
  m_file_size = obj.value("size").toInt();
  qDebug() << "Receiver: receive head: " << obj;
  TransferInfo info("Down", m_from, m_filename, m_file_size,
                    TransferState::Transfering, 0);
  emit addProgress(info);
}

void ReceiveTask::processPackageData(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->write(data);
    m_byte_read += data.size();
    TransferInfo info("Down", m_from, m_filename, m_file_size,
                      TransferState::Transfering,
                      m_byte_read * 100 / m_file_size);
    emit updateProgress(info);
    qDebug() << "Receiver: receive data " << data.size();
  }
}

void ReceiveTask::processPackageFinish(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->close();
    TransferInfo info("Down", m_from, m_filename, m_file_size,
                      TransferState::Transfering, 100);
    emit updateProgress(info);
    qDebug() << "Receiver: receive finish ";
  }
}