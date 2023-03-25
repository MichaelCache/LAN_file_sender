#include "host_info.h"
RemoteHostInfo::RemoteHostInfo(){}
RemoteHostInfo::RemoteHostInfo(const QHostAddress& ip, const QString& host_name,
                           const QString& os)
    : m_host_addr(ip), m_host_name(host_name), m_os(os) {}

RemoteHostInfo::~RemoteHostInfo() {}

bool RemoteHostInfo::operator==(const RemoteHostInfo& other) {
  return m_host_addr == other.m_host_addr && m_host_name == other.m_host_name &&
         m_os == other.m_os;
}

// QHostAddress RemoteServer::addr() const { return m_host_addr; }