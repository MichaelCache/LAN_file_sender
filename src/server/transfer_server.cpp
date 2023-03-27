#include "transfer_server.h"

#include "receive_task.h"
#include "setting.h"

TransferServer::TransferServer(/* args */) {
  m_pool = new QThreadPool(this);
  // How many threads I want at any given time
  // If there are more connections, they will be qued until a threads is closed
  m_pool->setMaxThreadCount(Setting::ins().m_max_thread);
}

TransferServer::~TransferServer() {}

void TransferServer::incomingConnection(qintptr socketDescriptor) {
  ReceiveTask *receive_task = new ReceiveTask(socketDescriptor);
  // Delete that object when you're done (instead of using signals and slots)
  receive_task->setAutoDelete(true);

  m_pool->start(receive_task);
}
