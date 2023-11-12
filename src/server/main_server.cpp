#include "main_server.h"

#include "setting.h"

MainServer::MainServer(QObject* parent) : QObject(parent) {
  m_host_detector = new HostBroadcaster(this);
  m_control_server = new ControlServer(this);
  m_transfer_server = new TransferServer(this);

  // host detector
  connect(m_host_detector, &HostBroadcaster::detectHostOnLine, this,
          &MainServer::detectHostOnLine);
  connect(m_host_detector, &HostBroadcaster::detectHostOffline, this,
          &MainServer::detectHostOffline);

  // server as sender
  connect(m_control_server, &ControlServer::sendFileBeAccepted, this,
          &MainServer::senderSendFileBeAccepted);
  // canceled file transfer by remote host
  connect(m_control_server, &ControlServer::sendFileBeCancelled, this,
          &MainServer::senderSendFileBeCanceled);
  // deny pending send file
  connect(m_control_server, &ControlServer::sendFileBeRejected, this,
          &MainServer::senderSendFileBeRejected);

  connect(m_transfer_server, &TransferServer::updateSendProgress, this,
          [this](TransferInfo i) {
            QVector<TransferInfo> info;
            info.append(i);
            emit this->updateSendProgress(info);
          });

  // server as reciever
  connect(m_control_server, &ControlServer::recieveFileInfo, this,
          reciverReciveFileInfo);
  connect(m_transfer_server, &TransferServer::updateReceiveProgress, this,
          [this](TransferInfo i) {
            QVector<TransferInfo> info;
            info.append(i);
            emit this->updateReciveProgress(info);
          });
}

void MainServer::start() {
  m_host_detector->start();
  m_control_server->start();
  m_transfer_server->start();
}

void MainServer::stop() {
  m_host_detector->stop();
  m_control_server->stop();
  m_transfer_server->stop();
}

const QVector<QHostAddress>& MainServer::hostIp() {
  return m_host_detector->hostIp();
}

void MainServer::senderSendFile(QVector<TransferInfo> info) {
  for (auto&& i : info) {
    m_send_pending_task.insert(i);
  }

  m_control_server->sendFileInfo(info, ControlSignal::InfoSend);
}

void MainServer::senderSendFileBeAccepted(QVector<TransferInfo> info) {
  QVector<int> found_index;
  // find pending send file info and mark those be accepted
  QVector<TransferInfo> send_task;
  for (auto&& i : info) {
    if (m_send_pending_task.count(i) &&
        m_send_pending_task.find(i)->m_state == TransferState::Pending) {
      auto& task = *m_send_pending_task.find(i);
      send_task.push_back(task);
      const_cast<TransferInfo&>(task).m_state = TransferState::Transfering;
    }
  }
  m_transfer_server->onSendFile(send_task);
}

void MainServer::senderSendFileBeRejected(QVector<TransferInfo> info) {
  for (auto&& i : info) {
    m_send_pending_task.erase(i);
  }
}

void MainServer::senderSendFileBeCanceled(QVector<TransferInfo> info) {
  // canceled file transfer by sender
  for (auto&& i : info) {
    m_send_pending_task.erase(i);
  }
  m_control_server->sendFileInfo(info, ControlSignal::CancelSend);
  m_transfer_server->onCancelSend(info);
}

void MainServer::senderSendFileFinished(QVector<TransferInfo> info) {
  for (auto&& i : info) {
    m_send_pending_task.erase(i);
  }
}

void MainServer::reciverAcceptFile(QVector<TransferInfo> info) {
  m_control_server->sendFileInfo(info, ControlSignal::AcceptSend);
}

void MainServer::reciverRejectFile(QVector<TransferInfo> info) {
  m_control_server->sendFileInfo(info, ControlSignal::RejectSend);
}

void MainServer::reciverCancelFile(QVector<TransferInfo> info) {
  m_control_server->sendFileInfo(info, ControlSignal::CancelSend);
}
