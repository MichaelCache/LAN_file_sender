#include "control_server_test.h"

#include <QMetaObject>

#include "server/control_server.h"

Q_DECLARE_METATYPE(QVector<FileInfo>)
Q_DECLARE_METATYPE(QHostAddress)

void ControlServerTest::initTestCase() {
  qRegisterMetaType<QVector<FileInfo>>();
  qRegisterMetaType<QHostAddress>();
  m_cs = new ControlServer();
}

void ControlServerTest::sendTest() {
  QSignalSpy spy(m_cs, &ControlServer::recieveFile);

  QVector<FileInfo> info_1{FileInfo{"test.txt", 60}, FileInfo{"test2", 1}};
  QVector<FileInfo> info_2{FileInfo{"test2", 60}, FileInfo{"t2", 15}};

  m_cs->sendFileInfo(info_1, QHostAddress::LocalHost);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  auto arguments = spy.takeFirst();
  auto infos = arguments.at(0).value<QVector<FileInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = info_1.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_fullname, send_info.m_fullname);
    QCOMPARE(recv_info.m_byte, send_info.m_byte);
    QCOMPARE(recv_info.m_id, send_info.m_id);
  }

  m_cs->sendFileInfo(info_2, QHostAddress::LocalHost);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  arguments = spy.takeFirst();
  infos = arguments.at(0).value<QVector<FileInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = info_2.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_fullname, send_info.m_fullname);
    QCOMPARE(recv_info.m_byte, send_info.m_byte);
    QCOMPARE(recv_info.m_id, send_info.m_id);
  }
}

void ControlServerTest::cancelTest() {
  QSignalSpy spy(m_cs, &ControlServer::cancelFile);

  QVector<FileInfo> info_1{FileInfo{"test.txt", 60}, FileInfo{"test2", 1}};
  QVector<FileInfo> info_2{FileInfo{"test.drf", 160}, FileInfo{"t3", 14}};

  m_cs->cancelFileInfo(info_1, QHostAddress::LocalHost);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  auto arguments = spy.takeFirst();
  auto infos = arguments.at(0).value<QVector<FileInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = info_1.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_fullname, send_info.m_fullname);
    QCOMPARE(recv_info.m_byte, send_info.m_byte);
    QCOMPARE(recv_info.m_id, send_info.m_id);
  }

  m_cs->cancelFileInfo(info_2, QHostAddress::LocalHost);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  arguments = spy.takeFirst();
  infos = arguments.at(0).value<QVector<FileInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = info_2.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_fullname, send_info.m_fullname);
    QCOMPARE(recv_info.m_byte, send_info.m_byte);
    QCOMPARE(recv_info.m_id, send_info.m_id);
  }
}

void ControlServerTest::accpetTest() {
  QSignalSpy spy(m_cs, &ControlServer::acceptFile);

  QVector<FileInfo> info_1{FileInfo{"test.txt", 60}, FileInfo{"test2", 1}};
  //    QVector<FileInfo> info_2{FileInfo{"test.drf", 160}, FileInfo{"t3", 14}};

  m_cs->acceptFileInfo(info_1, QHostAddress::LocalHost);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  auto arguments = spy.takeFirst();
  auto infos = arguments.at(0).value<QVector<FileInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = info_1.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_fullname, send_info.m_fullname);
    QCOMPARE(recv_info.m_byte, send_info.m_byte);
    QCOMPARE(recv_info.m_id, send_info.m_id);
  }
}

void ControlServerTest::denyTest() {
  QSignalSpy spy(m_cs, &ControlServer::denyFile);

  QVector<FileInfo> info_1{FileInfo{"test.txt", 60}, FileInfo{"test2", 1}};
  //    QVector<FileInfo> info_2{FileInfo{"test.drf", 160}, FileInfo{"t3", 14}};

  m_cs->denyFileInfo(info_1, QHostAddress::LocalHost);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  auto arguments = spy.takeFirst();
  auto infos = arguments.at(0).value<QVector<FileInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = info_1.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_fullname, send_info.m_fullname);
    QCOMPARE(recv_info.m_byte, send_info.m_byte);
    QCOMPARE(recv_info.m_id, send_info.m_id);
  }
}

void ControlServerTest::cleanupTestCase() { m_cs->deleteLater(); }

QTEST_MAIN(ControlServerTest)
