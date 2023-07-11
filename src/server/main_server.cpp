#include "main_server.h"

#include "setting.h"

MainServer::MainServer(QObject* parent) : QObject(parent) {
  m_host_detector = new HostBroadcaster(this);
  connect(&Setting::ins(), &Setting::updateSettings, m_host_detector,
          &HostBroadcaster::onUpdateHostInfo);
  connect(m_host_detector, &HostBroadcaster::detectNewHost, this,
          &MainServer::detectNewHost);
  connect(m_host_detector, &HostBroadcaster::detectHostOffline, this,
          &MainServer::detectHostOffline);

  m_control_server = new ControlServer(this);
  connect(m_control_server, &ControlServer::recieveFileInfo, this,
          &recieveFileInfo);

  m_transfer_server = new TransferServer(this);

  m_host_detector->broadcast(MsgType::New);
  m_control_server->listen(QHostAddress::Any, Setting::ins().m_file_info_port);
  m_transfer_server->listen(QHostAddress::Any,
                            Setting::ins().m_file_trans_port);
}

MainServer::~MainServer() {}

void MainServer::onSendFileInfos(const QVector<TransferInfo>& info,
                                 const QHostAddress& dst) {
  m_control_server->sendFileInfo(info, dst, ControlSignal::InfoSend);
}

void MainServer::onSendFiles(const QVector<TransferInfo>& files) {
  // m_control_server
}

void MainServer::onAcceptSend(const QVector<TransferInfo>& files) {}