#pragma once

#include <QSettings>
#include <QtGlobal>

class Setting : public QSettings {
 public:
  ~Setting();
  static Setting &ins();

 private:
  Setting(const QString &organization = QString(), QObject *parent = nullptr);

  quint16 mTransferPort{0};
  quint16 mBCInterval{0};
  qint32 mFileBuffSize{0};
  QString mDownloadDir;
  bool mReplaceExistingFile{false};
};
