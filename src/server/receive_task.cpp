#include "receive_task.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include "package_type.h"
#include "setting.h"

ReceiveTask::ReceiveTask(qintptr descriptor) : m_socket_descriptor(descriptor) {
  m_socket = new QTcpSocket(this);

  connect(m_socket, &QTcpSocket::readyRead, this, &ReceiveTask::onReadyRead);
  connect(m_socket, &QTcpSocket::disconnected, this,
          &ReceiveTask::onDisconnected);
}

ReceiveTask::~ReceiveTask() {}

void ReceiveTask::run() {
  if (m_socket->setSocketDescriptor(m_socket_descriptor)) {
  } else {
    quit();
  }
  QThread::run();
}

void ReceiveTask::onReadyRead() {
  m_buff.append(m_socket->readAll());
  while (m_buff.size()) {
    PackageSize size{0};
    memcpy(&size, m_buff.data(), sizeof(PackageSize));
    // m_receive_buffer.remove(0, sizeof(PackageSize));
    auto data = m_buff.mid(0, size);
    data.remove(0, sizeof(PackageSize));
    PackageType type;
    memcpy(&type, data.data(), sizeof(PackageType));
    data.remove(0, sizeof(PackageType));
    // Q_ASSERT(m_receive_buffer.size() == size);
    qDebug() << size;

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
  auto filename = QFileInfo(obj.value("name").toString());
  m_file = new QFile(filename.fileName());
  m_file->open(QIODevice::Append);
}

void ReceiveTask::processPackageData(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->write(data);
  }
}

void ReceiveTask::processPackageFinish(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->close();
  }
}