#include "control_server_test.h"

#include <QMetaObject>

#include "server/control_server.h"

Q_DECLARE_METATYPE(QVector<FileInfo>)
Q_DECLARE_METATYPE(QHostAddress)

void ControlServerTest::sendTest() {
  qRegisterMetaType<QVector<FileInfo>>();
  qRegisterMetaType<QHostAddress>();

  ControlServer* cs = new ControlServer(this);
  QSignalSpy spy(cs, &ControlServer::recieveFileInfo);

  QStringList files_1{"test.txt", "test2"};
  QStringList files_2{"t.d", "t2"};

  for (auto&& i : files_1) {
    QFile file(i);
    if (!file.exists()) {
      if (file.open(QIODevice::ReadWrite)) {
        file.close();
      }
    }
  }

  for (auto&& i : files_2) {
    QFile file(i);
    if (!file.exists()) {
      if (file.open(QIODevice::ReadWrite)) {
        file.close();
      }
    }
  }

  cs->onSendFile(files_1, QHostAddress::LocalHost);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  auto arguments = spy.takeFirst();
  auto infos = arguments.at(0).value<QVector<FileInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    QCOMPARE(infos.at(i).m_name, files_1.at(i));
    QCOMPARE(infos.at(i).m_byte, 0);
  }

  cs->onSendFile(files_2, QHostAddress::LocalHost);
  QVERIFY(spy.wait());
  QCOMPARE(spy.count(), 1);

  arguments = spy.takeFirst();
  infos = arguments.at(0).value<QVector<FileInfo>>();
  for (int i = 0; i < infos.size(); ++i) {
    QCOMPARE(infos.at(i).m_name, files_2.at(i));
    QCOMPARE(infos.at(i).m_byte, 0);
  }
}

QTEST_MAIN(ControlServerTest)
