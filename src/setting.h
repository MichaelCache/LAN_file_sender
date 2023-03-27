#pragma once

#include <QSettings>
#include <QtGlobal>

class Setting : public QSettings {
  Q_OBJECT
 public:
  ~Setting();
  static Setting &ins();

  qint32 m_file_buffer_size{0};
  QString m_download_dir;
  QString m_hostname{""};
  bool m_replace_exsit_file{false};
  int m_max_thread{5};

 private:
  Setting(const QString &organization = QString(), QObject *parent = nullptr);
};
