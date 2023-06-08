#include "control_server_test.h"

#include "server/control_server.h"

void ControlServerTest::send() {
  ControlServer cs;
  QSignalSpy spy(&cs, &ControlServer::recieveFileInfo);

  QStringList files{"test.txt", "test2"};

  for (auto&& i : files) {
    QFile file(i);
    if (!file.exists()) {
      if (file.open(QIODevice::ReadWrite)) {
        file.close();
      }
    }
  }

  cs.onSendFile(files, QHostAddress::LocalHost);

  QCOMPARE(spy.count(), 1);
  auto arguments = spy.takeFirst();
  //  auto infos = arguments.at(0).value<QVector<FileInfo>>();
  //  for (int i = 0; i < infos.size(); ++i) {
  //    QCOMPARE(infos.at(i).m_name, files.at(i));
  //    QCOMPARE(infos.at(i).m_byte, 0);
  //  }

  //  QVERIFY(true);
}

// void ControlServerTest::onRecieveInfos(const QVector<FileInfo>& filenames,
//                                        const QHostAddress& dst) {}

QTEST_MAIN(ControlServerTest)
