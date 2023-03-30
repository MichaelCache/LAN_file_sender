#include "send_task.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "setting.h"

SendTask::SendTask(const QHostAddress& host, const QString& filename)
    : m_dst(host), m_filename(filename) {
  m_socket = new QTcpSocket();

  connect(m_socket, &QTcpSocket::bytesWritten, this, &SendTask::onBytesWritten);
  connect(m_socket, &QTcpSocket::connected, this, &SendTask::onConnected);
  connect(m_socket, &QTcpSocket::disconnected, this, &SendTask::onDisconnected);
}

SendTask::~SendTask() {}

void SendTask::run() {
  if (m_socket) {
    m_socket->connectToHost(m_dst, Setting::ins().m_file_trans_port);
  } else {
    quit();
  }
}

void SendTask::onBytesWritten(qint64 byte) {
  Q_UNUSED(byte);
  // send file data
  if (!m_socket->bytesToWrite()) {
    sendFileData();
    if (!m_byte_remain) {
      sendFinish();
      m_socket->disconnectFromHost();
      m_socket->waitForDisconnected();
    }
  }
}
void SendTask::onConnected() { sendHeader(); }
void SendTask::onDisconnected() { quit(); }

void SendTask::sendHeader() {
  // open file
  m_send_file = new QFile(m_filename, this);
  bool ok = m_send_file->open(QIODevice::ReadOnly);
  if (ok) {
    m_file_size = m_send_file->size();
    m_byte_remain = m_file_size;
  }
  // send file header
  QString filename = m_send_file->fileName();
  auto filesize = m_send_file->size();

  QJsonObject obj(
      QJsonObject::fromVariantMap({{"name", filename}, {"size", filesize}}));

  QByteArray header_data(QJsonDocument(obj).toJson());

  QByteArray send_data = preparePackage(PackageType::Header, header_data);
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
    // emit mInfo->errorOcurred(tr("Error while reading file."));
    return;
  }

  m_byte_remain -= bytes_read;
  if (m_byte_remain < 0) m_byte_remain = 0;

  auto data = preparePackage(PackageType::Data, file_buffer);
  m_socket->write(data);
}

void SendTask::sendFinish() {
  m_send_file->close();
  auto data = preparePackage(PackageType::Finish);
  m_socket->write(data);
}
