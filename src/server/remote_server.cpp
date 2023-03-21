#include "remote_server.h"
RemoteServer::RemoteServer(){}
RemoteServer::RemoteServer(const QHostAddress& ip, const QString& host_name,
                           const QString& os)
    : m_host_addr(ip), m_host_name(host_name), m_os(os) {}

RemoteServer::~RemoteServer() {}

bool RemoteServer::operator==(const RemoteServer& other) {
  return m_host_addr == other.m_host_addr && m_host_name == other.m_host_name &&
         m_os == other.m_os;
}

// QHostAddress RemoteServer::addr() const { return m_host_addr; }