#pragma once

#include <QString>

class RemoteServer {
 public:
  RemoteServer(/* args */);
  ~RemoteServer();

 private:
  QString m_hostname;
  quint16 m_port;
};
