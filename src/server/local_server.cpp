#include "local_server.h"

#include <QStandardPaths>

LocalServer::LocalServer(QObject *parent) : QLocalServer(parent) {}

LocalServer::~LocalServer() {}

QString LocalServer::getDefaultDownloadPath() {
#if defined(Q_OS_WIN)
  return QStandardPaths::locate(QStandardPaths::DownloadLocation, QString(),
                                QStandardPaths::LocateDirectory) +
         "LANShareDownloads";
#else
  return QDir::homePath() + QDir::separator() + "LANShareDownloads";
#endif
}