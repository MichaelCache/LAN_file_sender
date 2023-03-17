#pragma once

#include <QLocalServer>
#include <QHostAddress>
#include <QUdpSocket>

class LocalServer : public QLocalServer {
 public:
  LocalServer(QObject *parent = nullptr);
  ~LocalServer();

  QString getDefaultDownloadPath();

 private:
  QHostAddress m_local_host;
  quint16 m_bc_port{0};
};
