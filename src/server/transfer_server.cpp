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

void TransferServer::start() {
  listen(QHostAddress::Any, Setting::ins().m_file_trans_port);
}

void TransferServer::stop() {
  // do nothing
}

void TransferServer::incomingConnection(qintptr socketDescriptor) {
  auto receiver = new ReceiveTask(socketDescriptor, this);
  // emit newReceiveTaskCreated(receiver);
  connect(receiver, &ReceiveTask::taskFinish, this,
          &TransferServer::removeReceive);
  connect(receiver, &ReceiveTask::updateProgress, this,
          &TransferServer::updateReceiveProgress);

  appendReceive(receiver->taskId(), receiver);
}

void TransferServer::onSendFile(QVector<TransferInfo> info) {
  for (auto&& i : info) {
    auto sender = new SendTask(i);
    // emit newSendTaskCreated(sender);
    auto& task = sender->task();
    connect(sender, &SendTask::taskFinish, this, &TransferServer::removeSend);
    connect(sender, &SendTask::updateProgress, this,
            &TransferServer::updateSendProgress);
    appendSend(i.id(), sender);
  }
}

void TransferServer::onCancelSend(QVector<TransferInfo> info) {
  QMutexLocker locker(&m_lock);
  for (auto&& i : info) {
    auto task_id = i.id();
    SendTask* sender;
    auto iter =
        std::find_if(m_sender_wait_queue.begin(), m_sender_wait_queue.end(),
                     [task_id](const SendTask* task) {
                       if (task->taskId() == task_id) {
                         return true;
                       }
                       return false;
                     });
    if (iter != m_sender_wait_queue.end()) {
      sender = *iter;
      m_sender_wait_queue.erase(iter);
      sender->onCancelSend();
    }

    iter = std::find_if(m_senders.begin(), m_senders.end(),
                        [task_id](const SendTask* task) {
                          if (task->taskId() == task_id) {
                            return true;
                          }
                          return false;
                        });

    if (iter != m_sender_wait_queue.end()) {
      sender = *iter;
      m_senders.erase(iter);
      sender->onCancelSend();
      // bump wait queue
      if ((quint32)m_senders.size() < Setting::ins().m_max_send_thread &&
          m_sender_wait_queue.size()) {
        auto wait_sender = m_sender_wait_queue.front();
        m_sender_wait_queue.pop_front();
        m_senders.append(wait_sender);
        wait_sender->run();
      }
    }
  }
}

void TransferServer::removeSend(const QUuid& task_id) {
  QMutexLocker locker(&m_lock);
  SendTask* sender;
  auto iter =
      std::find_if(m_sender_wait_queue.begin(), m_sender_wait_queue.end(),
                   [task_id](const SendTask* task) {
                     if (task->taskId() == task_id) {
                       return true;
                     }
                     return false;
                   });
  if (iter != m_sender_wait_queue.end()) {
    sender = *iter;
    m_sender_wait_queue.erase(iter);
    sender->quit();
  }

  iter = std::find_if(m_senders.begin(), m_senders.end(),
                      [task_id](const SendTask* task) {
                        if (task->taskId() == task_id) {
                          return true;
                        }
                        return false;
                      });

  if (iter != m_sender_wait_queue.end()) {
    sender = *iter;
    m_senders.erase(iter);
    sender->quit();
    // bump wait queue
    if ((quint32)m_senders.size() < Setting::ins().m_max_send_thread &&
        m_sender_wait_queue.size()) {
      auto wait_sender = m_sender_wait_queue.front();
      m_sender_wait_queue.pop_front();
      m_senders.append(wait_sender);
      wait_sender->run();
    }
  }
}

void TransferServer::removeReceive(const QUuid& task_id) {
  QMutexLocker locker(&m_lock);
  ReceiveTask* receiver;
  auto iter =
      std::find_if(m_receiver_wait_queue.begin(), m_receiver_wait_queue.end(),
                   [task_id](const ReceiveTask* task) {
                     if (task->taskId() == task_id) {
                       return true;
                     }
                     return false;
                   });
  if (iter != m_receiver_wait_queue.end()) {
    receiver = *iter;
    m_receiver_wait_queue.erase(iter);
    receiver->quit();
  }

  iter = std::find_if(m_receivers.begin(), m_receivers.end(),
                      [task_id](const ReceiveTask* task) {
                        if (task->taskId() == task_id) {
                          return true;
                        }
                        return false;
                      });

  if (iter != m_receivers.end()) {
    receiver = *iter;
    m_receivers.erase(iter);
    receiver->quit();
    // bump wait queue
    if ((quint32)m_receivers.size() < Setting::ins().m_max_receive_thread &&
        m_receiver_wait_queue.size()) {
      auto wait_sender = m_receiver_wait_queue.front();
      m_receiver_wait_queue.pop_front();
      m_receivers.append(wait_sender);
      wait_sender->run();
    }
  }
}

void TransferServer::appendSend(const TaskId& id, SendTask* sender) {
  QMutexLocker locker(&m_lock);
  if ((quint32)m_senders.size() < Setting::ins().m_max_send_thread) {
    m_senders.append(sender);
    sender->run();
  } else {
    m_sender_wait_queue.append(sender);
  }
}

void TransferServer::appendReceive(const TaskId& id, ReceiveTask* receiver) {
  QMutexLocker locker(&m_lock);
  if ((quint32)m_receivers.size() < Setting::ins().m_max_receive_thread) {
    m_receivers.append(receiver);
    receiver->run();
  } else {
    m_receiver_wait_queue.append(receiver);
  }
}

void TransferServer::bumpSendQueue() {
  QMutexLocker locker(&m_lock);
  if ((quint32)m_senders.size() < Setting::ins().m_max_send_thread &&
      m_sender_wait_queue.size()) {
    auto wait_sender = m_sender_wait_queue.front();
    m_sender_wait_queue.pop_front();
    m_senders.append(wait_sender);
    wait_sender->run();
  }
}

void TransferServer::bumpReceiveQueue() {
  QMutexLocker locker(&m_lock);
  if ((quint32)m_receivers.size() < Setting::ins().m_max_receive_thread &&
      m_receiver_wait_queue.size()) {
    auto wait_sender = m_receiver_wait_queue.front();
    m_receiver_wait_queue.pop_front();
    m_receivers.append(wait_sender);
    wait_sender->run();
  }
}
