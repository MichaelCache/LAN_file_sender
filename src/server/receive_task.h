#pragma once

#include <QByteArray>
#include <QFile>
#include <QRunnable>
#include <QTcpSocket>

#include "model/transfer_info.h"
#include "package_type.h"


class ReceiveTask : public QRunnable {
  // Q_OBJECT
 private:
  /* data */
 public:
  ReceiveTask(int descriptor);
  ~ReceiveTask();

  virtual void run() override;

//  Q_SIGNALS:
//   void error(const QString&);
//   void receiveNewFile(TransferInfo&);

//  private Q_SLOTS:
  // void onReadyRead();
  // void onDisconnected();

 private:
  void processPackage(QByteArray& data, PackageType type);
  void processPackageHeader(QByteArray& data);

  QTcpSocket* m_receive_tcp;
  int m_socket_descriptor{0};
  QByteArray m_buff;
  QFile* m_file;
};
