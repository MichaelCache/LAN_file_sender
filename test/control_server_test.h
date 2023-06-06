#pragma once
#include <QObject>
#include <QString>

#include "doctest.h"
#include "server/control_server.h"

class MocTest : public QObject {
  Q_OBJECT
 public:
  MocTest();
  ~MocTest();

  void run();

 public Q_SLOTS:
  void acceptSendFile(const QVector<FileInfo>& filenames,
                      const QHostAddress& dst);
  void denySendFile(const QVector<FileInfo>& filenames,
                    const QHostAddress& dst);
  void recieveFileInfo(const QVector<FileInfo>& filenames,
                       const QHostAddress& dst);
  void cancelFileInfo(const QVector<FileInfo>& filenames,
                      const QHostAddress& dst);

 private:
  ControlServer* cs;
};