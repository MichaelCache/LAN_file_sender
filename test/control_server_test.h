#pragma once

#include <QtTest/QtTest>

class ControlServerTest : public QObject {
  Q_OBJECT

  // public Q_SLOTS:
  //  void onRecieveInfos(const QVector<FileInfo>& filenames,
  //                      const QHostAddress& dst);
 private slots:
  void send();
  // void case2_name();
  // void case3_consting();
  // void case4_price();
  // void case5_profit();
};
