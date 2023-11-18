#pragma once

#include <QFile>
#include <QTcpSocket>
#include <QThread>


#include "transfer_info.h"

class SendTask : public QThread {
  Q_OBJECT
 public:
  SendTask(const TransferInfo& info, QObject* parent = nullptr);
  ~SendTask() = default;

  virtual void run() override;

  QUuid taskId() const;

 Q_SIGNALS:
  void updateProgress(const TransferInfo&);
  void taskFinish(QUuid);

 public Q_SLOTS:
  void onCancelSendTask();

 private Q_SLOTS:
  // void onBytesWritten(qint64 byte);
  void onConnected();
  void onDisconnected();

 private:
  void sendHeader();
  void sendFileData();
  void sendFinish();
  void sendCanceled();

  void exitDelete();

  QFile* m_send_file{nullptr};
  QTcpSocket* m_socket;
  quint64 m_byte_remain{0};
  TransferInfo m_transinfo;
};
