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

TransferServer::TransferServer(QObject* parent) : QTcpServer(parent) {}

TransferServer::~TransferServer() {}

void TransferServer::incomingConnection(qintptr socketDescriptor) {
  auto receiver = new ReceiveTask(socketDescriptor, this);
  emit newReceiveTaskCreated(receiver);
  connect(receiver, &ReceiveTask::taskFinish, this,
          &TransferServer::removeReceive);
  connect(receiver, &ReceiveTask::updateProgress, this,
          &TransferServer::updateReceiveProgress);

  appendReceive(receiver->taskId(), receiver);
}

void TransferServer::onSendFile(const QVector<TransferInfo>& info) {
  for (auto&& i : info) {
    auto sender = new SendTask(i);
    emit newSendTaskCreated(sender);
    auto& task = sender->task();
    connect(sender, &SendTask::taskFinish, this, &TransferServer::removeSend);
    connect(sender, &SendTask::updateProgress, this,
            &TransferServer::updateSendProgress);
    appendSend(i.id(), sender);
  }
}

void TransferServer::onCancelSend(const QVector<TransferInfo>& info) {
  QMutexLocker locker(&m_lock);
  for (auto&& i : info) {
    auto task_id = i.id();
    SendTask* sender;
    auto iter =
        std::find_if(m_sender_wait_queue.begin(), m_sender_wait_queue.end(),
                     [task_id](const QPair<TaskId, SendTask*> pair) {
                       if (pair.first == task_id) {
                         return true;
                       }
                       return false;
                     });
    if (iter != m_sender_wait_queue.end()) {
      sender = iter->second;
      m_sender_wait_queue.erase(iter);
      sender->onCancelSend();
    }

    sender = m_senders.value(task_id);
    if (sender) {
      m_senders.remove(task_id);
      sender->onCancelSend();
      // bump wait queue
      if ((quint32)m_senders.size() < Setting::ins().m_max_send_thread &&
          m_sender_wait_queue.size()) {
        auto wait_sender = m_sender_wait_queue.front();
        m_sender_wait_queue.pop_front();
        m_senders.insert(wait_sender.first, wait_sender.second);
        wait_sender.second->run();
      }
    }
  }
}

void TransferServer::removeSend(const QUuid& task_id) {
  QMutexLocker locker(&m_lock);
  SendTask* sender;
  auto iter =
      std::find_if(m_sender_wait_queue.begin(), m_sender_wait_queue.end(),
                   [task_id](const QPair<TaskId, SendTask*> pair) {
                     if (pair.first == task_id) {
                       return true;
                     }
                     return false;
                   });
  if (iter != m_sender_wait_queue.end()) {
    sender = iter->second;
    m_sender_wait_queue.erase(iter);
    sender->quit();
  }

  sender = m_senders.value(task_id);
  if (sender) {
    m_senders.remove(task_id);
    sender->quit();
    // bump wait queue
    if ((quint32)m_senders.size() < Setting::ins().m_max_send_thread &&
        m_sender_wait_queue.size()) {
      auto wait_sender = m_sender_wait_queue.front();
      m_sender_wait_queue.pop_front();
      m_senders.insert(wait_sender.first, wait_sender.second);
      wait_sender.second->run();
    }
  }
}

void TransferServer::removeReceive(const QUuid& task_id) {
  QMutexLocker locker(&m_lock);
  ReceiveTask* receiver;
  auto iter =
      std::find_if(m_receiver_wait_queue.begin(), m_receiver_wait_queue.end(),
                   [task_id](const QPair<TaskId, ReceiveTask*> pair) {
                     if (pair.first == task_id) {
                       return true;
                     }
                     return false;
                   });
  if (iter != m_receiver_wait_queue.end()) {
    receiver = iter->second;
    m_receiver_wait_queue.erase(iter);
    receiver->quit();
  }

  receiver = m_receivers.value(task_id);
  if (receiver) {
    m_receivers.remove(task_id);
    receiver->quit();
    // bump wait queue
    if ((quint32)m_receivers.size() < Setting::ins().m_max_receive_thread &&
        m_receiver_wait_queue.size()) {
      auto wait_sender = m_receiver_wait_queue.front();
      m_receiver_wait_queue.pop_front();
      m_receivers.insert(wait_sender.first, wait_sender.second);
      wait_sender.second->run();
    }
  }
}

void TransferServer::appendSend(const TaskId& id, SendTask* sender) {
  QMutexLocker locker(&m_lock);
  if ((quint32)m_senders.size() < Setting::ins().m_max_send_thread) {
    m_senders.insert(id, sender);
    sender->run();
  } else {
    m_sender_wait_queue.push_back({id, sender});
  }
}

void TransferServer::appendReceive(const TaskId& id, ReceiveTask* receiver) {
  QMutexLocker locker(&m_lock);
  if ((quint32)m_receivers.size() < Setting::ins().m_max_receive_thread) {
    m_receivers.insert(id, receiver);
    receiver->run();
  } else {
    m_receiver_wait_queue.push_back({id, receiver});
  }
}

void TransferServer::bumpSendQueue() {
  QMutexLocker locker(&m_lock);
  if ((quint32)m_senders.size() < Setting::ins().m_max_send_thread &&
      m_sender_wait_queue.size()) {
    auto wait_sender = m_sender_wait_queue.front();
    m_sender_wait_queue.pop_front();
    m_senders.insert(wait_sender.first, wait_sender.second);
    wait_sender.second->run();
  }
}

void TransferServer::bumpReceiveQueue() {
  QMutexLocker locker(&m_lock);
  if ((quint32)m_receivers.size() < Setting::ins().m_max_receive_thread &&
      m_receiver_wait_queue.size()) {
    auto wait_sender = m_receiver_wait_queue.front();
    m_receiver_wait_queue.pop_front();
    m_receivers.insert(wait_sender.first, wait_sender.second);
    wait_sender.second->run();
  }
}
