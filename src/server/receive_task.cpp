#include "receive_task.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include "package_type.h"
#include "setting.h"

ReceiveTask::ReceiveTask(qintptr descriptor, QObject* parent)
    : QThread(parent), m_socket_descriptor(descriptor) {
  m_transinfo.m_type = "Download";
}

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
  if (m_file && m_file->isOpen()) {
    m_file->close();
  }
  if (m_transinfo.m_state == TransferState::Transfering ||
      m_transinfo.m_state == TransferState::Waiting) {
    m_transinfo.m_state = TransferState::Disconnected;
    emit updateProgress(m_transinfo);
  }
  exitDelete();
}

void ReceiveTask::processPackage(PackageType type, QByteArray& data) {
  switch (type) {
    case PackageType::Header:
      processPackageHeader(data);
      break;
    case PackageType::Data:
      processPackageData(data);
      break;
    case PackageType::Cancel:
      processPackageCancel(data);
      m_socket->disconnectFromHost();
      exitDelete();
      break;
      ;
    case PackageType::Finish:
      processPackageFinish(data);
      m_socket->disconnectFromHost();
      exitDelete();
      break;
    default:
      break;
  }
}

void ReceiveTask::processPackageHeader(QByteArray& data) {
  QJsonObject obj = QJsonDocument::fromJson(data).object();
  auto filename = obj.value("name").toString();
  m_file = new QFile(filename);
  m_file->open(QIODevice::Append);
  auto from_ip = QHostAddress(m_socket->peerAddress().toIPv4Address());
  auto file_size = obj.value("size").toInt();
  // qDebug() << "Receiver: receive head: " << obj;

  m_transinfo.m_dest_ip = from_ip;
  m_transinfo.m_file_name = filename;
  m_transinfo.m_file_size = file_size;
  m_transinfo.m_state = TransferState::Waiting;
  m_transinfo.m_progress = 0;
  emit addProgress(m_transinfo);
}

void ReceiveTask::processPackageData(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->write(data);
    m_byte_read += data.size();
    auto progress = m_byte_read * 100 / m_transinfo.m_file_size;
    m_transinfo.m_state = TransferState::Transfering;
    m_transinfo.m_progress = progress;
    emit updateProgress(m_transinfo);
    // qDebug() << "Receiver: receive data " << data.size();
  }
}

void ReceiveTask::processPackageFinish(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->close();
  }
  m_transinfo.m_state = TransferState::Finish;
  m_transinfo.m_progress = 100;
  emit updateProgress(m_transinfo);
}

void ReceiveTask::processPackageCancel(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->close();
  }
  m_transinfo.m_state = TransferState::Cancelled;
  emit updateProgress(m_transinfo);
}

void ReceiveTask::exitDelete() {
  quit();
  deleteLater();
}