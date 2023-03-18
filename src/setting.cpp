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

QString &Setting::hostName() { return m_hostname; }

QString Setting::getDefaultDownloadPath() {
  return QStandardPaths::locate(QStandardPaths::DownloadLocation, QString(),
                                QStandardPaths::LocateDirectory);
}
