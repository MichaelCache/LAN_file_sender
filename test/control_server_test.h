#pragma once

#include <QtTest/QtTest>

#include "server/control_server.h"

class ControlServerTest : public QObject {
  Q_OBJECT
 private slots:
  void initTestCase();
  void sendTest();
  void cancelTest();
  void accpetTest();
  void denyTest();
  void reconnectSendTest();
  void cleanupTestCase();

 private:
  ControlServer* m_cs;
};
