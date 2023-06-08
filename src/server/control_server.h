#pragma once

#include <QHash>
#include <QHostAddress>
#include <QMutex>
#include <QTcpServer>
#include <QVector>
#include <tuple>

#include "control_state.h"
#include "transfer_server.h"

class ControlServer : public QTcpServer {
  Q_OBJECT
 public:
  ControlServer(QObject* parent = nullptr);
  ~ControlServer();

 public Q_SLOTS:
  void onSendFile(const QStringList& filenames, const QHostAddress& dst);
  void onCancelSend(const QStringList& filenames, const QHostAddress& dst);
  // void onAcceptSend(const QVector<FileInfo>& filenames, const QHostAddress&
  // src);

 Q_SIGNALS:
  void acceptSendFile(const QVector<FileInfo>& filenames,
                      const QHostAddress& dst);
  void denySendFile(const QVector<FileInfo>& filenames,
                    const QHostAddress& dst);
  void recieveFileInfo(const QVector<FileInfo>& filenames,
                       const QHostAddress& dst);
  void cancelFileInfo(const QVector<FileInfo>& filenames,
                      const QHostAddress& dst);

 protected:
  void incomingConnection(qintptr socket_descriptor);

 private:
  QByteArray packFileInfoPackage(ControlSignal, const QStringList&);
  std::tuple<ControlSignal, QVector<FileInfo>> unpackFileInfoPackage(
      QByteArray&);

  QVector<FileInfo> fileListToFileInfo(const QStringList&);

  QTcpSocket* getSender(const QStringList& filenames,
                        const QHostAddress& address);
  QTcpSocket* getReciever(const qintptr& descriptor);

  // data
  QHash<QHostAddress, QTcpSocket*> m_info_sender;
  QMap<qintptr, QTcpSocket*> m_info_reciever;
  QMap<qintptr, QByteArray> m_info_recieve_cache;
  TransferServer* m_transfer_server;
};
