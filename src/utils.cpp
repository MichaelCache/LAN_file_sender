#include "utils.h"

QString getDefaultDownloadPath() {
  return QStandardPaths::locate(QStandardPaths::DownloadLocation, QString(),
                                QStandardPaths::LocateDirectory);
}