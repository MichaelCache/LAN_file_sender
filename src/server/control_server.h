#pragma once

#include <QHash>
#include <QHostAddress>
#include <QMutex>
#include <QTcpServer>
#include <QVector>
#include <tuple>

#include "fileinfo.h"
#include "setting.h"
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
  ControlServer(QObject* parent = nullptr,
                quint32 file_info_port = Setting::ins().m_file_info_port);
  ~ControlServer();

 public Q_SLOTS:
  void sendFileInfo(const QVector<FileInfo>& info, const QHostAddress& dst);
  void cancelFileInfo(const QVector<FileInfo>& info, const QHostAddress& dst);
  void acceptFileInfo(const QVector<FileInfo>& info, const QHostAddress& src);
  void denyFileInfo(const QVector<FileInfo>& info, const QHostAddress& src);

 Q_SIGNALS:
  void acceptFile(const QVector<FileInfo>& filenames, const QHostAddress& dst);
  void denyFile(const QVector<FileInfo>& filenames, const QHostAddress& dst);
  void recieveFile(const QVector<FileInfo>& filenames, const QHostAddress& dst);
  void cancelFile(const QVector<FileInfo>& filenames, const QHostAddress& dst);

 protected:
  void incomingConnection(qintptr socket_descriptor);

 private:
  QByteArray packFileInfoPackage(ControlSignal, const QVector<FileInfo>&);
  std::tuple<ControlSignal, QVector<FileInfo>> unpackFileInfoPackage(
      QByteArray&);

  void send(const QVector<FileInfo>& info, const QHostAddress& address,
            const ControlSignal& signal);

  // data
  QHash<QHostAddress, QTcpSocket*> m_info_sender;
  QMap<qintptr, QTcpSocket*> m_info_reciever;
  QMap<qintptr, QByteArray> m_info_recieve_cache;
  quint32 m_file_info_port;
};
