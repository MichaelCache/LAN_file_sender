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

 private:
  ControlServer* m_cs;
};
