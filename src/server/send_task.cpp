#include "send_task.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

#include "setting.h"

SendTask::SendTask(const QHostAddress& host, const QString& filename,
                   QObject* parent)
    : QThread(parent), m_dst(host), m_filename(filename) {
  m_status.m_type = "Upload";
  m_status.m_dest_ip = host;
}

SendTask::~SendTask() { qDebug() << "Sender thread deconstruct"; }

void SendTask::run() {
  m_socket = new QTcpSocket(this);

  connect(m_socket, &QTcpSocket::bytesWritten, this, &SendTask::onBytesWritten);
  connect(m_socket, &QTcpSocket::connected, this, &SendTask::onConnected);
  connect(m_socket, &QTcpSocket::disconnected, this, &SendTask::onDisconnected);
  m_socket->connectToHost(m_dst, Setting::ins().m_file_trans_port);
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
  m_send_file = new QFile(m_filename, static_cast<QThread*>(this));
  bool ok = m_send_file->open(QIODevice::ReadOnly);
  qint64 file_size;
  if (ok) {
    file_size = m_send_file->size();
    m_byte_remain = file_size;
  } else {
    qDebug() << "Open File failed " << m_filename;
    return;
  }
  // send file header
  auto filename = QFileInfo(m_send_file->fileName());
  auto pare_filename = filename.fileName();

  QJsonObject obj(QJsonObject::fromVariantMap(
      {{"name", pare_filename}, {"size", file_size}}));

  QByteArray header_data(QJsonDocument(obj).toJson());

  QByteArray send_data = preparePackage(PackageType::Header, header_data);
  m_socket->write(send_data);
  // qDebug() << "Sender: send header " << obj;
  m_status.m_file_name = pare_filename;
  m_status.m_file_size = file_size;
  m_status.m_state = TransferState::Waiting;
  m_status.m_progress = 0;
  emit addProgress(m_status);
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

  auto data = preparePackage(PackageType::Data, file_buffer);
  m_socket->write(data);
  // qDebug() << "Sender: send data ";
  m_status.m_state = TransferState::Transfering;
  auto progress =
      (m_status.m_file_size - m_byte_remain) * 100 / m_status.m_file_size;
  m_status.m_progress = progress;
  emit updateProgress(m_status);
}

void SendTask::sendFinish() {
  m_send_file->close();
  auto data = preparePackage(PackageType::Finish);
  m_socket->write(data);
  // qDebug() << "Sender: send finish ";
  m_status.m_state = TransferState::Finish;
  m_status.m_progress = 100;
  emit updateProgress(m_status);
}
