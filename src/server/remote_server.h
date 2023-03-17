#pragma once

#include <QHostAddress>
#include <QString>

class RemoteServer {
 public:
  RemoteServer(/* args */);
  ~RemoteServer();

 private:
  QHostAddress m_host_addr;
  QString m_username;
  quint16 m_port;
};
