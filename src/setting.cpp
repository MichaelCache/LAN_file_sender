#include "setting.h"

#include <QDir>
#include <QStandardPaths>

#include "config.h"

Setting::Setting(const QString &organization, QObject *parent)
    : QSettings(organization, APP_NAME, parent) {
  m_download_dir = getDefaultDownloadPath();
}

Setting::~Setting() {}

Setting &Setting::ins() {
  static Setting s;
  return s;
}

QString Setting::getDefaultDownloadPath() {
#if defined(Q_OS_WIN)
  return QStandardPaths::locate(QStandardPaths::DownloadLocation, QString(),
                                QStandardPaths::LocateDirectory);
#else
  return QDir::homePath() + QDir::separator() + "Downloads";
#endif
}
