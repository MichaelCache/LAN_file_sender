#include "send_task.h"

#include <QDataStream>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>

#include "setting.h"

SendTask::SendTask(const QHostAddress& host, const QString& filename,
                   QObject* parent)
    : QThread(parent), m_dst(host), m_filename(filename) {
  m_send_file = new QFile(m_filename, static_cast<QThread*>(this));
  bool ok = m_send_file->open(QIODevice::ReadOnly);
  qint64 file_size = m_send_file->size();
  m_byte_remain = file_size;
  auto fileinfo = QFileInfo(m_send_file->fileName());
  auto bare_filename = fileinfo.fileName();

  m_transinfo.m_type = "Upload";
  m_transinfo.m_dest_ip = host;
  m_transinfo.m_state = TransferState::Waiting;
  m_transinfo.m_file_name = bare_filename;
  m_transinfo.m_file_size = file_size;
  m_transinfo.m_state = TransferState::Waiting;
  m_transinfo.m_progress = 0;
  // emit addProgress(m_transinfo);
}

SendTask::~SendTask() {}

void SendTask::run() {
  m_socket = new QTcpSocket(this);

  connect(m_socket, &QTcpSocket::bytesWritten, this, &SendTask::onBytesWritten);
  connect(m_socket, &QTcpSocket::connected, this, &SendTask::onConnected);
  connect(m_socket, &QTcpSocket::disconnected, this, &SendTask::onDisconnected);
  m_socket->connectToHost(m_dst, Setting::ins().m_file_trans_port);
}

qintptr SendTask::taskId() const { return m_transinfo.id(); }

const TransferInfo SendTask::task() const { return m_transinfo; }

void SendTask::onCancelSend() {
  m_transinfo.m_state = TransferState::Cancelled;
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
void SendTask::onConnected() { sendHeader(); }

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
  auto name_data = m_transinfo.m_file_name.toUtf8();
  int name_size = name_data.size();
  header_buffer.append((char*)&name_size, sizeof(int));
  header_buffer.append(name_data);
  header_buffer.append((char*)&m_transinfo.m_file_size, sizeof(quint64));

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
  // qDebug() << "Sender: send data ";
  m_transinfo.m_state = TransferState::Transfering;
  auto progress =
      (m_transinfo.m_file_size - m_byte_remain) * 100 / m_transinfo.m_file_size;
  m_transinfo.m_progress = progress;
  emit updateProgress(m_transinfo);
}

void SendTask::sendFinish() {
  m_send_file->close();
  auto data = TcpPackage::packData(PackageType::Finish);
  m_socket->write(data);
  // qDebug() << "Sender: send finish ";
  m_transinfo.m_state = TransferState::Finish;
  m_transinfo.m_progress = 100;
  emit updateProgress(m_transinfo);
  emit taskFinish(m_transinfo.id());
}

void SendTask::sendCancelled() {
  m_send_file->close();
  auto data = TcpPackage::packData(PackageType::Cancel);
  m_socket->write(data);
  // qDebug() << "Sender: send finish ";
  m_transinfo.m_state = TransferState::Cancelled;
  // m_transinfo.m_progress = 100;
  emit updateProgress(m_transinfo);
  emit taskFinish(m_transinfo.id());
}

void SendTask::exitDelete() {
  quit();
  deleteLater();
}