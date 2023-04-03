#pragma once

#include <QHostAddress>
#include <QString>

class RemoteHostInfo {
 public:
  RemoteHostInfo();
  RemoteHostInfo(const QHostAddress& ip, const QString& host_name = QString(),
               const QString& os = QString());
  ~RemoteHostInfo();

  bool operator==(const RemoteHostInfo&);
  // QHostAddress addr() const;

  //  private:
  QHostAddress m_host_addr;
  QString m_host_name;
  QString m_os;
};
