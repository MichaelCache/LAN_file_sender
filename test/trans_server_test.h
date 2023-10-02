#pragma once

#include <QTest>

#include "server/transfer_server.h"

class TransServerTest : public QObject {
  Q_OBJECT
 private slots:
  void initTestCase();
  void sendTest();
  void cancelTest();
  void accpetTest();
  void denyTest();
  void cleanupTestCase();

 private:
  TransferServer* m_ts;
};
