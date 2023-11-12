#pragma once

#include <QHash>
#include <QHostAddress>
#include <QMutex>
#include <QTcpServer>
#include <QVector>
#include <tuple>

#include "fileinfo.h"
#include "setting.h"
#include "transfer_info.h"
#include "transfer_server.h"

enum class ControlSignal : int {
  None = 0,
  InfoSend,
  CancelSend,
  AcceptSend,
  RejectSend,
};

class ControlServer : public QTcpServer {
  Q_OBJECT
 public:
  using QTcpServer::QTcpServer;
  ~ControlServer() = default;

  void start();
  void stop();

 public Q_SLOTS:
  void sendFileInfo(QVector<TransferInfo> info, ControlSignal signal,
                    qint32 send_port = Setting::ins().m_file_info_port);

 Q_SIGNALS:
  void sendFileBeAccepted(const QVector<TransferInfo>& trans_info);
  void sendFileBeRejected(const QVector<TransferInfo>& trans_info);
  void recieveFileInfo(const QVector<TransferInfo>& trans_info);
  void sendFileBeCancelled(const QVector<TransferInfo>& trans_info);

 protected:
  using descriptor = qintptr;
  void incomingConnection(descriptor socket_descriptor);

 private:
  QByteArray packFileInfoPackage(ControlSignal, const QVector<FileInfo>&);
  std::tuple<ControlSignal, QVector<FileInfo>> unpackFileInfoPackage(
      QByteArray&);

  void clearDisconnectedReciver();

  // data
  QHash<QHostAddress, QTcpSocket*> m_info_sender;
  QMap<descriptor, QTcpSocket*> m_info_reciever;
  QMap<descriptor, QByteArray> m_info_recieve_cache;
};
