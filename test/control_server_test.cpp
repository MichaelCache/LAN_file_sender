#include "control_server_test.h"

#include <QMetaObject>

#include "server/control_server.h"

Q_DECLARE_METATYPE(QVector<TransferInfo>)
Q_DECLARE_METATYPE(QHostAddress)

void ControlServerTest::initTestCase() {
  qRegisterMetaType<QVector<TransferInfo>>();
  qRegisterMetaType<QHostAddress>();
  m_cs = new ControlServer(this);
  m_cs->listen(QHostAddress::Any, Setting::ins().m_file_info_port);

  TransferInfo f1, f2;
  f1.m_file_name = "test.txt";
  f1.m_file_size = 60;
  f2.m_file_name = "test2";
  f2.m_file_size = 1;
  m_info_1.push_back(f1);
  m_info_1.push_back(f2);

  TransferInfo f3, f4;
  f3.m_file_name = "test2";
  f3.m_file_size = 60;
  f4.m_file_name = "t2";
  f4.m_file_size = 15;
  m_info_2.push_back(f1);
  m_info_2.push_back(f2);
}

void ControlServerTest::sendTest() {
  QSignalSpy spy(m_cs, &ControlServer::recieveFileInfo);

  m_cs->sendFileInfo(m_info_1, ControlSignal::InfoSend);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  auto arguments = spy.takeFirst();
  auto infos = arguments.at(0).value<QVector<TransferInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = m_info_1.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_file_name, send_info.m_file_name);
    QCOMPARE(recv_info.m_file_size, send_info.m_file_size);
    QCOMPARE(recv_info.id(), send_info.id());
  }

  m_cs->sendFileInfo(m_info_2, ControlSignal::InfoSend);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  arguments = spy.takeFirst();
  infos = arguments.at(0).value<QVector<TransferInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = m_info_2.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_file_name, send_info.m_file_name);
    QCOMPARE(recv_info.m_file_size, send_info.m_file_size);
    QCOMPARE(recv_info.id(), send_info.id());
  }
}

void ControlServerTest::cancelTest() {
  QSignalSpy spy(m_cs, &ControlServer::sendFileCancelled);

  m_cs->sendFileInfo(m_info_1, ControlSignal::CancelSend);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  auto arguments = spy.takeFirst();
  auto infos = arguments.at(0).value<QVector<TransferInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = m_info_1.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_file_name, send_info.m_file_name);
    QCOMPARE(recv_info.m_file_size, send_info.m_file_size);
    QCOMPARE(recv_info.id(), send_info.id());
  }

  m_cs->sendFileInfo(m_info_2, ControlSignal::CancelSend);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  arguments = spy.takeFirst();
  infos = arguments.at(0).value<QVector<TransferInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = m_info_2.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_file_name, send_info.m_file_name);
    QCOMPARE(recv_info.m_file_size, send_info.m_file_size);
    QCOMPARE(recv_info.id(), send_info.id());
  }
}

void ControlServerTest::accpetTest() {
  QSignalSpy spy(m_cs, &ControlServer::sendFileAccepted);

  m_cs->sendFileInfo(m_info_1, ControlSignal::AcceptSend);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  auto arguments = spy.takeFirst();
  auto infos = arguments.at(0).value<QVector<TransferInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = m_info_1.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_file_name, send_info.m_file_name);
    QCOMPARE(recv_info.m_file_size, send_info.m_file_size);
    QCOMPARE(recv_info.id(), send_info.id());
  }
}

void ControlServerTest::denyTest() {
  QSignalSpy spy(m_cs, &ControlServer::sendFiledenied);

  m_cs->sendFileInfo(m_info_1, ControlSignal::DenySend);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  auto arguments = spy.takeFirst();
  auto infos = arguments.at(0).value<QVector<TransferInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    auto& send_info = m_info_1.at(i);
    auto& recv_info = infos.at(i);
    QCOMPARE(recv_info.m_file_name, send_info.m_file_name);
    QCOMPARE(recv_info.m_file_size, send_info.m_file_size);
    QCOMPARE(recv_info.id(), send_info.id());
  }
}

void ControlServerTest::reconnectSendTest() {
  auto sender = new ControlServer();
  auto reciver = new ControlServer();
  qint32 port = 11715;

  reciver->listen(QHostAddress::Any, port);
  QSignalSpy spy(reciver, &ControlServer::sendFileAccepted);

  sender->sendFileInfo(m_info_1, ControlSignal::AcceptSend, port);

  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  delete reciver;

  // wait reciver socket be disconnected for scoket change state async;
  QTestEventLoop loop;
  loop.enterLoop(3);

  auto reciver2 = new ControlServer();
  QSignalSpy spy1(reciver2, &ControlServer::sendFileAccepted);

  reciver2->listen(QHostAddress::Any, port);
  sender->sendFileInfo(m_info_2, ControlSignal::AcceptSend, port);

  QVERIFY(spy1.wait());
  QCOMPARE(spy1.count(), 1);
}

void ControlServerTest::cleanupTestCase() {}

QTEST_MAIN(ControlServerTest)
