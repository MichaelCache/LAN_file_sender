#pragma once

#include <QFile>
#include <QHostAddress>
#include <QTcpSocket>
#include <QThread>

#include "tcp_package.h"

class SendTask : public QThread, public TcpPackage {
  Q_OBJECT
 public:
  SendTask(const QHostAddress& host, const QString& filename);
  ~SendTask();

  virtual void run() override;

 private Q_SLOTS:
  void onBytesWritten(qint64 byte);
  void onConnected();
  void onDisconnected();

 private:
  void sendHeader();
  void sendFileData();
  void sendFinish();

  QFile* m_send_file{nullptr};
  QTcpSocket* m_socket;
  quint64 m_file_size{0};
  quint64 m_byte_remain{0};
  QHostAddress m_dst;
  QString m_filename;
};
