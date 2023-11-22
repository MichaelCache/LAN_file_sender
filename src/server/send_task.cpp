#include "send_task.h"

#include <QDataStream>

#include "setting.h"
#include "tcp_package.h"

SendTask::SendTask(const TransferInfo& info, QObject* parent)
    : QThread(parent), m_transinfo(info) {
  m_byte_remain = m_transinfo.m_file_size;
  m_transinfo.m_state = TransferState::Pending;
  m_transinfo.m_progress = 0;

  m_send_file = new QFile(m_transinfo.m_file_path, this);

  m_socket = new QTcpSocket(this);
  connect(m_socket, &QTcpSocket::bytesWritten, this, &SendTask::onBytesWritten);
  connect(m_socket, &QTcpSocket::connected, this, &SendTask::onConnected);
  connect(m_socket, &QTcpSocket::disconnected, this, &SendTask::onDisconnected);
}

void SendTask::run() {
  m_socket->connectToHost(m_transinfo.m_dest_ip,
                          Setting::ins().m_file_trans_port);
}

QUuid SendTask::taskId() const { return m_transinfo.id(); }

// void SendTask::onCancelSendTask() {
//   if (m_transinfo.m_state == TransferState::Disconnected ||
//       m_transinfo.m_state == TransferState::Finish ||
//       m_transinfo.m_state == TransferState::Rejected) {
//     return;
//   }
//   m_transinfo.m_state = TransferState::Canceled;
//   sendCanceled();
//   emit updateProgress(m_transinfo);
//   exitDelete();
// }

void SendTask::onBytesWritten(qint64 byte) {
  Q_UNUSED(byte);
  // send file data
  if (!m_socket->bytesToWrite()) {
    if (m_transinfo.m_state == TransferState::Pending ||
        m_transinfo.m_state == TransferState::Transfering) {
      if (m_byte_remain) {
        sendFileData();
      } else {
        sendFinish();
      }
    }
  }
}

void SendTask::onConnected() {
  m_send_file->open(QIODevice::ReadOnly);
  sendHeader();
}

/**
 * @brief Receiver offline
 *
 */
void SendTask::onDisconnected() {
  if (m_transinfo.m_state == TransferState::Transfering ||
      m_transinfo.m_state == TransferState::Pending) {
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
}

void SendTask::sendCanceled() {
  auto data = TcpPackage::packData(PackageType::Cancel);
  m_socket->write(data);
  m_transinfo.m_state = TransferState::Canceled;
  emit updateProgress(m_transinfo);
  exitDelete();
}

void SendTask::exitDelete() {
  if (m_send_file && m_send_file->isOpen()) {
    m_send_file->close();
  }
  emit taskFinish(m_transinfo.id());
  deleteLater();
  quit();
}
