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

void TransferServer::onSendFile(const QString& filename,
                                const QHostAddress& dst) {
  auto sender = new SendTask(dst, filename);
  auto& task = sender->task();
  m_progress_model->add(task);
  //   connect(sender, &QThread::finished, sender, &QThread::deleteLater);
  connect(sender, &SendTask::taskFinish, this, &TransferServer::removeThread);
  connect(sender, &SendTask::updateProgress, m_progress_model,
          &ProgressModel::update);
  //   connect(sender, &SendTask::removeProgress, m_progress_model,
  //           &ProgressModel::remove);

  //   connect(this,&TransferServer::cancelSend, sender,
  //   &SendTask::onCancelSend);

  appendSend(sender);
}

void TransferServer::onCancelSend(qintptr taskid) {
  auto sender = m_senders.value(taskid);
  if (sender) {
    sender->onCancelSend();
  }
}

void TransferServer::removeThread(qintptr taskid) {
  QMutexLocker locker(&m_lock);
  m_senders.remove(taskid);
  if (m_sender_wait_queue.size()) {
    auto sender = m_sender_wait_queue.front();
    m_sender_wait_queue.pop_front();
    m_senders.insert(sender->taskId(), sender);
    sender->run();
  }
}

void TransferServer::appendSend(SendTask* sender) {
  if (m_senders.size() < Setting::ins().m_max_send_thread) {
    m_senders.insert(sender->taskId(), sender);
    sender->run();
  } else {
    m_sender_wait_queue.push_back(sender);
  }
}