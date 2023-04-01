#include "transfer_server.h"

#include <QByteArray>
#include <QDataStream>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>

#include "config.h"
#include "receive_task.h"
#include "setting.h"

TransferServer::TransferServer(QObject* parent) : QTcpServer(parent) {
  // How many threads I want at any given time
  // If there are more connections, they will be qued until a threads is closed
}

TransferServer::~TransferServer() {}

void TransferServer::incomingConnection(qintptr socketDescriptor) {
  auto receiver = new ReceiveTask(socketDescriptor);
  m_receivers.push_back(receiver);
  receiver->run();
  // Delete that object when you're done (instead of using signals and slots)
  // receive_task->setAutoDelete(true);

  // m_pool->start(receive_task);
}

void TransferServer::sendFile(const QString& filename,
                              const QHostAddress& dst) {
  // mInfo->setFilePath(mFilePath);
  auto sender = new SendTask(dst, filename);
  m_senders.push_back(sender);
  sender->run();
}
