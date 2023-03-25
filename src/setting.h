#pragma once

#include <QSettings>
#include <QtGlobal>

class Setting : public QSettings {
  Q_OBJECT
 public:
  ~Setting();
  static Setting &ins();

  QString &hostName();

 private:
  Setting(const QString &organization = QString(), QObject *parent = nullptr);

  QString getDefaultDownloadPath();

  qint32 m_file_buffer_size{0};
  QString m_download_dir;
  QString m_hostname{""};
  bool m_replace_exsit_file{false};
  quint8 m_max_thread{5};
};
