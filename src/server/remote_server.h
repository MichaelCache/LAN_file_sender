#pragma once

#include <QHostAddress>
#include <QStandardItem>
#include <QString>

class RemoteServer : public QStandardItem {
 public:
  RemoteServer(const QHostAddress& ip, const QString& host_name = QString(),
               const QString& os = QString());
  ~RemoteServer();

  QHostAddress addr() const;

 private:
  QHostAddress m_host_addr;
  QString m_host_name;
  QString m_os;
};
