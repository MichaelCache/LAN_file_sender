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

  quint16 mTransferPort{0};
  qint32 mFileBuffSize{0};
  QString m_download_dir;
  QString m_hostname{""};
  bool mReplaceExistingFile{false};
};
