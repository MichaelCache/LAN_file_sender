#pragma once

#include <QHostAddress>
#include <QString>

class RemoteServer {
 public:
  RemoteServer(const QHostAddress& ip, const QString& host_name = QString(),
               const QString& os = QString());
  ~RemoteServer();

 private:
  QHostAddress m_host_addr;
  QString m_host_name;
  QString m_os;
};
