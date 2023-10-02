#include "send_task.h"

#include <QDataStream>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>

#include "setting.h"

SendTask::SendTask(const TransferInfo& info, QObject* parent)
    : QThread(parent), m_transinfo(info) {
  m_byte_remain = m_transinfo.m_file_size;
  m_transinfo.m_state = TransferState::Waiting;
  m_transinfo.m_progress = 0;

  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this,
          [this]() { this->updateProgress(m_transinfo); });
}

SendTask::~SendTask() {}

void SendTask::run() {
  m_socket = new QTcpSocket(this);
  m_send_file = new QFile(m_transinfo.m_file_path, static_cast<QThread*>(this));
  connect(m_socket, &QTcpSocket::bytesWritten, this, &SendTask::onBytesWritten);
  connect(m_socket, &QTcpSocket::connected, this, &SendTask::onConnected);
  connect(m_socket, &QTcpSocket::disconnected, this, &SendTask::onDisconnected);
  m_socket->connectToHost(m_transinfo.m_dest_ip,
                          Setting::ins().m_file_trans_port);
  // report transfer progress per 0.1 sec
  m_timer->start(100);
}

QUuid SendTask::taskId() const { return m_transinfo.id(); }

const TransferInfo SendTask::task() const { return m_transinfo; }

void SendTask::onCancelSend() {
  if (m_transinfo.m_state == TransferState::Disconnected ||
      m_transinfo.m_state == TransferState::Finish) {
    return;
  }
  m_transinfo.m_state = TransferState::Cancelled;
  emit updateProgress(m_transinfo);
}

void SendTask::onBytesWritten(qint64 byte) {
  Q_UNUSED(byte);
  // send file data
  if (!m_socket->bytesToWrite()) {
    if (m_transinfo.m_state == TransferState::Waiting ||
        m_transinfo.m_state == TransferState::Transfering) {
      sendFileData();
      if (!m_byte_remain) {
        sendFinish();
        // wait rececive socket disconnect;
      }
    } else if (m_transinfo.m_state == TransferState::Cancelled) {
      sendCancelled();
      // wait rececive socket disconnect;
    }
  }
}
void SendTask::onConnected() {
  sendHeader();
  m_send_file->open(QIODevice::ReadOnly);
}

/**
 * @brief Receiver offline
 *
 */
void SendTask::onDisconnected() {
  if (m_transinfo.m_state == TransferState::Transfering ||
      m_transinfo.m_state == TransferState::Waiting) {
    m_transinfo.m_state = TransferState::Disconnected;
    emit updateProgress(m_transinfo);
  }
  exitDelete();
}

void SendTask::sendHeader() {
  QByteArray header_buffer;
  QDataStream s(&header_buffer, QIODevice::WriteOnly);
  s << m_transinfo.m_file_name << m_transinfo.m_file_size << m_transinfo.m_id;
  QByteArray send_data =
      TcpPackage::packData(PackageType::Header, header_buffer);
  m_socket->write(send_data);
}

void SendTask::sendFileData() {
  QByteArray file_buffer;
  auto buffer_size = Setting::ins().m_file_buffer_size;
  if (m_byte_remain < buffer_size) {
    file_buffer.resize(m_byte_remain);
  } else {
    file_buffer.resize(buffer_size);
  }

  qint64 bytes_read = m_send_file->read(file_buffer.data(), file_buffer.size());
  if (bytes_read == -1) {
    return;
  }

  m_byte_remain -= bytes_read;
  if (m_byte_remain < 0) m_byte_remain = 0;

  auto data = TcpPackage::packData(PackageType::Data, file_buffer);
  m_socket->write(data);
  m_transinfo.m_state = TransferState::Transfering;
  auto progress =
      (m_transinfo.m_file_size - m_byte_remain) * 100 / m_transinfo.m_file_size;
  m_transinfo.m_progress = progress;
  emit updateProgress(m_transinfo);
}

void SendTask::sendFinish() {
  auto data = TcpPackage::packData(PackageType::Finish);
  m_socket->write(data);
  m_transinfo.m_state = TransferState::Finish;
  m_transinfo.m_progress = 100;
  emit updateProgress(m_transinfo);
  emit taskFinish(m_transinfo.id());
  exitDelete();
}

void SendTask::sendCancelled() {
  auto data = TcpPackage::packData(PackageType::Cancel);
  m_socket->write(data);
  m_transinfo.m_state = TransferState::Cancelled;
  emit updateProgress(m_transinfo);
  emit taskFinish(m_transinfo.id());
  exitDelete();
}

void SendTask::exitDelete() {
  if (m_send_file && m_send_file->isOpen()) {
    m_send_file->close();
  }
  deleteLater();
  quit();
}
