#include "setting.h"

#include <QDir>
#include <QHostInfo>
#include <QStandardPaths>

#include "config.h"
#include "utils.h"

Setting::Setting(QObject *parent) : QSettings(ORG_NAME, APP_NAME, parent) {
  m_download_dir = getDefaultDownloadPath();
  loadSettingFile();
}

Setting &Setting::ins() {
  static Setting s;
  return s;
}

void Setting::loadSettingFile() {
  m_hostname = value(HOST_NAME, QHostInfo::localHostName()).toString();
  m_file_buffer_size =
      value(FILE_BUFFER, DefaultFileBufferSize).value<quint32>();
  m_max_send_thread = value(SEND_THREAD, DefaultMaxSendThread).value<quint32>();
  m_max_receive_thread =
      value(RECE_THREAD, DefaultMaxReceiveThread).value<quint32>();
  m_download_dir = value(DOWN_DIR, getDefaultDownloadPath()).toString();
  m_boradcast_interval =
      value(BROAD_INTER, DefaultBroadcastInterval).value<quint32>();
  m_file_trans_port = value(SEND_PORT, DefaultTransferPort).value<quint32>();
  m_replace_exsit_file = value(REPLACE, true).toBool();

  if (!QDir(m_download_dir).exists()) {
    QDir dir;
    dir.mkpath(m_download_dir);
  }
}

void Setting::saveSettings() {
  setValue(HOST_NAME, m_hostname);
  setValue(FILE_BUFFER, m_file_buffer_size);
  setValue(SEND_THREAD, m_max_send_thread);
  setValue(RECE_THREAD, m_max_receive_thread);
  setValue(DOWN_DIR, m_download_dir);
  setValue(SEND_PORT, m_file_trans_port);
  setValue(REPLACE, m_replace_exsit_file);
  setValue(BROAD_INTER, m_boradcast_interval);
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
  m_boradcast_interval = DefaultBroadcastInterval;
  // emit updateSettings();
}
