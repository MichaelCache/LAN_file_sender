#pragma once

#include <QSettings>
#include <QtGlobal>

#include "config.h"

class Setting : public QSettings {
  Q_OBJECT
 public:
  ~Setting();
  static Setting &ins();

  quint32 m_file_buffer_size{DefaultFileBufferSize};
  QString m_download_dir;
  QString m_hostname{""};
  bool m_replace_exsit_file{false};
  quint32 m_max_send_thread{5};
  quint32 m_max_receive_thread{5};
  quint32 m_file_trans_port{DefaultTransferPort};

 private:
  Setting(const QString &organization = QString(), QObject *parent = nullptr);
  QString getDefaultDownloadPath();
};
