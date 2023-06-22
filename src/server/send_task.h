#pragma once

#include <QFile>
#include <QHostAddress>
#include <QMutex>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

#include "fileinfo.h"
#include "model/transfer_info.h"
#include "tcp_package.h"

class SendTask : public QThread, public TcpPackage {
  Q_OBJECT
 public:
  SendTask(const QHostAddress& host, const FileInfo& info,
           QObject* parent = nullptr);
  ~SendTask();

  virtual void run() override;

  QUuid taskId() const;
  const TransferInfo task() const;

 Q_SIGNALS:
  void updateProgress(const TransferInfo&);
  void taskFinish(QUuid);

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

  QFile* m_send_file{nullptr};
  FileInfo m_info;
  QTcpSocket* m_socket;
  quint64 m_byte_remain{0};
  QHostAddress m_dst;
  TransferInfo m_transinfo;
  QTimer* m_timer;
};
