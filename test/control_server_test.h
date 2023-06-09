#pragma once

#include <QtTest/QtTest>

class ControlServerTest : public QObject {
  Q_OBJECT
 private slots:
  void sendTest();
};
