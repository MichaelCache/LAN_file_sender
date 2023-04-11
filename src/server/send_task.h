#pragma once

#include <QFile>
#include <QHostAddress>
#include <QMutex>
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

  qintptr taskId() const;
  const TransferInfo task() const;

 Q_SIGNALS:
  void updateProgress(const TransferInfo&);
  void taskFinish(qintptr);

 public Q_SLOTS:
  void onCancelSend();

 private Q_SLOTS:
  void onBytesWritten(qint64 byte);
  void onConnected();
  void onDisconnected();

 private:
  void sendHeader();
  void sendFileData();
  void sendFinish();
  void sendCancelled();

  void exitDelete();

  QMutex m_lock;
  QFile* m_send_file{nullptr};
  QString m_filename;
  QTcpSocket* m_socket;
  quint64 m_byte_remain{0};
  QHostAddress m_dst;
  TransferInfo m_transinfo;
};
