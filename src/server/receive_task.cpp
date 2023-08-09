#include "receive_task.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include "setting.h"
#include "tcp_package.h"

ReceiveTask::ReceiveTask(qintptr descriptor, QObject* parent)
    : QThread(parent), m_socket_descriptor(descriptor) {
  m_socket = new QTcpSocket(this);
  connect(m_socket, &QTcpSocket::readyRead, this, &ReceiveTask::onReadyRead);
  connect(m_socket, &QTcpSocket::disconnected, this,
          &ReceiveTask::onDisconnected);

  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this,
          [this]() { this->updateProgress(m_transinfo); });
}

ReceiveTask::~ReceiveTask() {}

void ReceiveTask::run() { m_socket->setSocketDescriptor(m_socket_descriptor); }

QUuid ReceiveTask::taskId() const { return m_transinfo.id(); }

void ReceiveTask::onReadyRead() {
  m_buff.append(m_socket->readAll());
  while (m_buff.size()) {
    auto tcp_pack = TcpPackage::unpackData(m_buff);
    // not valid tcp package, wait receive more data package
    if (tcp_pack.m_type == PackageType::None) {
      break;
    }

    processPackage(tcp_pack.m_type, tcp_pack.m_data);
    m_buff.remove(0, tcp_pack.m_size);
  }
}

void ReceiveTask::onDisconnected() {
  if (m_file && m_file->isOpen()) {
    m_file->close();
    m_file = nullptr;
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
      break;
      ;
    case PackageType::Finish:
      processPackageFinish(data);
      break;
    default:
      qDebug() << "error type";
      break;
  }
}

void ReceiveTask::processPackageHeader(QByteArray& data) {
  QDataStream s(data);
  QString filename;
  quint64 file_size;
  QUuid id;
  s >> filename >> file_size >> id;

  auto full_name = QDir(Setting::ins().m_download_dir).filePath(filename);
  m_file = new QFile(full_name);
  m_file->open(QIODevice::Append);
  auto from_ip = QHostAddress(m_socket->peerAddress().toIPv4Address());

  m_transinfo.m_from_ip = from_ip;
  m_transinfo.m_file_path = full_name;
  m_transinfo.m_file_name = filename;
  m_transinfo.m_file_size = file_size;
  m_transinfo.m_state = TransferState::Waiting;
  m_transinfo.m_progress = 0;
  m_transinfo.m_id = id;
  emit addProgress(m_transinfo);
  m_timer->start(100);
}

void ReceiveTask::processPackageData(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->write(data);
    m_byte_read += data.size();
    auto progress = m_byte_read * 100 / m_transinfo.m_file_size;
    m_transinfo.m_state = TransferState::Transfering;
    m_transinfo.m_progress = progress;
    emit updateProgress(m_transinfo);
  }
}

void ReceiveTask::processPackageFinish(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->close();
  }
  m_transinfo.m_state = TransferState::Finish;
  m_transinfo.m_progress = 100;
  emit updateProgress(m_transinfo);
  emit taskFinish(m_transinfo.id());
}

void ReceiveTask::processPackageCancel(QByteArray& data) {
  if (m_file && m_file->isOpen()) {
    m_file->close();
  }
  m_transinfo.m_state = TransferState::Cancelled;
  emit updateProgress(m_transinfo);
  emit taskFinish(m_transinfo.id());
}

void ReceiveTask::exitDelete() {
  if (m_file && m_file->isOpen()) {
    m_file->close();
  }

  deleteLater();
  quit();
}
