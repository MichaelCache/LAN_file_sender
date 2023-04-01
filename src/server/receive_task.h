#pragma once

#include <QByteArray>
#include <QFile>
#include <QTcpSocket>
#include <QThread>

#include "model/transfer_info.h"
#include "package_type.h"

class ReceiveTask : public QThread {
  Q_OBJECT
 private:
  /* data */
 public:
  ReceiveTask(qintptr descriptor, QObject* parent = nullptr);
  ~ReceiveTask();

  virtual void run() override;

  //  Q_SIGNALS:
  //   void error(const QString&);
  //   void receiveNewFile(TransferInfo&);

 private Q_SLOTS:
  void onReadyRead();
  void onDisconnected();

 private:
  void processPackage(PackageType type, QByteArray& data);
  void processPackageHeader(QByteArray& data);
  void processPackageData(QByteArray& data);
  void processPackageFinish(QByteArray& data);

  QTcpSocket* m_socket;
  qintptr m_socket_descriptor{0};
  QByteArray m_buff;
  QFile* m_file;
};
