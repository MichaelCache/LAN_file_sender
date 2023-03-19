#include "remote_server.h"
RemoteServer::RemoteServer(const QHostAddress& ip, const QString& host_name,
                           const QString& os)
    : QStandardItem(), m_host_addr(ip), m_host_name(host_name), m_os(os) {}

RemoteServer::~RemoteServer() {}

// QHostAddress RemoteServer::addr() const { return m_host_addr; }