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
  connect(m_control_server, &ControlServer::acceptFile, m_transfer_server,
          &TransferServer::onSendFile);
  // canceled file transfer by remote host
  connect(m_control_server, &ControlServer::cancelFile, m_transfer_server,
          &TransferServer::onCancelSend);
  // when control server get accept signal, trans server can send file now
  connect(m_control_server, &ControlServer::denyFile, m_transfer_server,
          &TransferServer::onCancelSend);
  connect(m_control_server, &ControlServer::denyFile, this,
          &MainServer::sendFileDenied);
  connect(m_transfer_server, &TransferServer::updateSendProgress, this,
          &MainServer::updateSendProgress);

  // server as reciever
  connect(m_control_server, &ControlServer::recieveFileInfo, this,
          &MainServer::recieveFileInfo);
  connect(m_transfer_server, &TransferServer::updateReceiveProgress, this,
          &MainServer::updateReceiveProgress);
}

MainServer::~MainServer() {}

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

void MainServer::onSendFile(QVector<TransferInfo> info) {
  m_control_server->sendFileInfo(info, ControlSignal::InfoSend);
}

void MainServer::onSendCancelFile(QVector<TransferInfo> info) {
  // canceled file transfer by sender
  m_control_server->sendFileInfo(info, ControlSignal::CancelSend);
  m_transfer_server->onCancelSend(info);
}
