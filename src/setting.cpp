#include "setting.h"

#include <QDir>
#include <QHostInfo>
#include <QStandardPaths>

#include "config.h"

Setting::Setting(QObject *parent) : QSettings(ORG_NAME, APP_NAME, parent) {
  m_download_dir = getDefaultDownloadPath();
  loadSettingFile();
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

void Setting::loadSettingFile() {
  m_hostname = value(HOST_NAME, QHostInfo::localHostName()).toString();
  m_file_buffer_size =
      value(FILE_BUFFER, DefaultFileBufferSize).value<quint16>();
  m_max_send_thread = value(SEND_THREAD, DefaultMaxSendThread).toUInt();
  m_max_receive_thread = value(RECE_THREAD, DefaultMaxReceiveThread).toUInt();
  m_download_dir = value(DOWN_DIR, getDefaultDownloadPath()).toString();

  if (!QDir(m_download_dir).exists()) {
    QDir dir;
    dir.mkpath(m_download_dir);
  }

  m_file_trans_port = value(SEND_PORT, DefaultTransferPort).value<quint16>();
  m_replace_exsit_file = value(REPLACE, true).toBool();
}

void Setting::saveSettings() {
  setValue(HOST_NAME, m_hostname);
  setValue(FILE_BUFFER, m_file_buffer_size);
  setValue(SEND_THREAD, m_max_send_thread);
  setValue(RECE_THREAD, m_max_receive_thread);
  setValue(DOWN_DIR, m_download_dir);
  setValue(SEND_PORT, m_file_trans_port);
  setValue(REPLACE, m_replace_exsit_file);
  emit updateSettings();
}

void Setting::reset() {
  m_hostname = QHostInfo::localHostName();
  m_max_send_thread = DefaultMaxSendThread;
  m_max_receive_thread = DefaultMaxReceiveThread;
  m_file_trans_port = DefaultTransferPort;
  m_file_buffer_size = DefaultFileBufferSize;
  m_download_dir = getDefaultDownloadPath();
  m_replace_exsit_file = true;
  emit updateSettings();
}
