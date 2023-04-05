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
  m_progress_model = new ProgressModel(this);
  // How many threads I want at any given time
  // If there are more connections, they will be qued until a threads is closed
}

TransferServer::~TransferServer() {}

ProgressModel* TransferServer::progressModel() { return m_progress_model; }

void TransferServer::incomingConnection(qintptr socketDescriptor) {
  auto receiver = new ReceiveTask(socketDescriptor);
  // m_receivers.push_back(receiver);
  connect(receiver, &QThread::finished, receiver, &QThread::deleteLater);
  connect(receiver, &ReceiveTask::addProgress, m_progress_model,
          &ProgressModel::add);
  connect(receiver, &ReceiveTask::updateProgress, m_progress_model,
          &ProgressModel::update);
  connect(receiver, &ReceiveTask::removeProgress, m_progress_model,
          &ProgressModel::remove);
  receiver->run();
}

void TransferServer::sendFile(const QString& filename,
                              const QHostAddress& dst) {
  auto sender = new SendTask(dst, filename);
  // m_senders.push_back(sender);
  connect(sender, &QThread::finished, sender, &QThread::deleteLater);
  connect(sender, &SendTask::addProgress, m_progress_model,
          &ProgressModel::add);
  connect(sender, &SendTask::updateProgress, m_progress_model,
          &ProgressModel::update);
  connect(sender, &SendTask::removeProgress, m_progress_model,
          &ProgressModel::remove);
  sender->run();
}
