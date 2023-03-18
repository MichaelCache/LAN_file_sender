#include "remote_server.h"
RemoteServer::RemoteServer(const QHostAddress& ip, const QString& host_name,
                           const QString& os)
    : m_host_addr(ip), m_host_name(host_name), m_os(os) {}

RemoteServer::~RemoteServer() {}
