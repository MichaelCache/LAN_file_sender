#include "receive_task.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include "package_type.h"
#include "setting.h"

ReceiveTask::ReceiveTask(int descriptor) : m_socket_descriptor(descriptor) {
  m_receive_tcp = new QTcpSocket();
}

ReceiveTask::~ReceiveTask() {}

void ReceiveTask::run() {
  if (m_receive_tcp->setSocketDescriptor(m_socket_descriptor)) {
    m_buff.append(m_receive_tcp->readAll());
    while (m_buff.size() > sizeof(PackageSize) + sizeof(PackageType)) {
      PackageType type = static_cast<PackageType>(m_buff.at(0));
      processPackage(m_buff, type);
    }
  }
}

// void ReceiveTask::onReadyRead() { m_buff.append(m_receive_tcp->readAll()); }
// void ReceiveTask::onDisconnected() {}

void ReceiveTask::processPackage(QByteArray& data, PackageType type) {
  switch (type) {
    case PackageType::Header:
      processPackageHeader(data);
      break;

    default:
      break;
  }
}

void ReceiveTask::processPackageHeader(QByteArray& data) {
  TransferInfo info;
  QJsonObject obj = QJsonDocument::fromJson(data).object();
  auto file_size = obj.value("size").toVariant().value<qint64>();
  info.m_file_size = file_size;

  QString filename = obj.value("name").toString();
  info.m_file_name = filename;
  QString foldername = obj.value("folder").toString();
  QString download_dir = Setting::ins().m_download_dir;
  if (!foldername.isEmpty())
    download_dir = download_dir + QDir::separator() + foldername;

  // mkdir if not exist
  QDir dir(download_dir);
  if (!dir.exists()) {
    dir.mkpath(download_dir);
  }

  QString download_file_dir = download_dir + QDir::separator() + filename;

  // if (!Setting::ins()->getReplaceExistingFile()) {
  //   download_file_dir = Util::getUniqueFileName(filename, download_dir);
  // }

  // mInfo->setFilePath(download_file_dir);
  m_file = new QFile(download_file_dir, this);
  if (m_file->open(QIODevice::WriteOnly)) {
    info.m_state = TransferState::Transfering;
    emit receiveNewFile(info);
  } else {
    emit error("Failed to write " + download_file_dir);
  }
}