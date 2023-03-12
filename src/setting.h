#pragma once

#include <QSettings>

class Setting : public QSettings {
 public:
  ~Setting();
  static Setting& ins();

 private:
  Setting(/* args */);

  quint16 mTransferPort{0};
  quint16 mBCInterval{0};
  qint32 mFileBuffSize{0};
  QString mDownloadDir;
  bool mReplaceExistingFile{false};
};

