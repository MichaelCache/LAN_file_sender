#pragma once

#include <QSettings>
#include <QtGlobal>

#include "config.h"

class Setting : public QSettings {
  Q_OBJECT
 public:
  ~Setting();
  static Setting &ins();

  void saveSettings();
  void reset();

  quint32 m_file_buffer_size{DefaultFileBufferSize};
  QString m_download_dir;
  QString m_hostname{""};
  bool m_replace_exsit_file{false};
  quint32 m_max_send_thread{DefaultMaxSendThread};
  quint32 m_max_receive_thread{DefaultMaxReceiveThread};
  quint32 m_file_trans_port{DefaultTransferPort};

 Q_SIGNALS:
  void updateSettings();

 private:
  Setting(QObject *parent = nullptr);
  QString getDefaultDownloadPath();
  void loadSettingFile();
  
};
