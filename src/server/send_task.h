#pragma once

#include <QFile>
#include <QHostAddress>
#include <QTcpSocket>
#include <QThread>

#include "model/transfer_info.h"
#include "tcp_package.h"

class SendTask : public QThread, public TcpPackage {
  Q_OBJECT
 public:
  SendTask(const QHostAddress& host, const QString& filename,
           QObject* parent = nullptr);
  ~SendTask();

  virtual void run() override;

 Q_SIGNALS:
  void addProgress(const TransferInfo&);
  void removeProgress(const TransferInfo&);
  void updateProgress(const TransferInfo&);

 private Q_SLOTS:
  void onBytesWritten(qint64 byte);
  void onConnected();
  void onDisconnected();

 private:
  void sendHeader();
  void sendFileData();
  void sendFinish();

  QFile* m_send_file{nullptr};
  QString m_filename;
  QTcpSocket* m_socket;
  quint64 m_file_size{0};
  quint64 m_byte_remain{0};
  QHostAddress m_dst;
};
