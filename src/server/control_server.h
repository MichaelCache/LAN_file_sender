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
  DenySend,
};

class ControlServer : public QTcpServer {
  Q_OBJECT
 public:
  ControlServer(QObject* parent = nullptr);
  ~ControlServer();

 public Q_SLOTS:
  void sendFileInfo(const QVector<TransferInfo>& info, const QHostAddress& dst,
                    const ControlSignal& signal,
                    qint32 send_port = Setting::ins().m_file_info_port);

 Q_SIGNALS:
  void acceptFile(const QVector<TransferInfo>& trans_info);
  void denyFile(const QVector<TransferInfo>& trans_info);
  void recieveFileInfo(const QVector<TransferInfo>& trans_info);
  void cancelFile(const QVector<TransferInfo>& trans_info);

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
