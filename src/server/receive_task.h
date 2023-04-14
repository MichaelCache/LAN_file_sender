#pragma once

#include <QByteArray>
#include <QFile>
#include <QTcpSocket>
#include <QThread>

#include "model/transfer_info.h"
#include "tcp_package.h"

class ReceiveTask : public QThread {
  Q_OBJECT
 private:
  /* data */
 public:
  ReceiveTask(qintptr descriptor, QObject* parent = nullptr);
  ~ReceiveTask();

  virtual void run() override;

  qintptr taskId() const;

 Q_SIGNALS:
  void addProgress(const TransferInfo&);
  // void removeProgress(const TransferInfo&);
  void updateProgress(const TransferInfo&);
  void taskFinish(qintptr);

 private Q_SLOTS:
  void onReadyRead();
  void onDisconnected();

 private:
  void processPackage(PackageType type, QByteArray& data);
  void processPackageHeader(QByteArray& data);
  void processPackageData(QByteArray& data);
  void processPackageFinish(QByteArray& data);
  void processPackageCancel(QByteArray& data);

  void exitDelete();

  QTcpSocket* m_socket;
  quint64 m_byte_read{0};
  qintptr m_socket_descriptor{0};
  QByteArray m_buff;
  QFile* m_file;
  TransferInfo m_transinfo;
};
