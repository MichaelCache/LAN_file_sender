#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// #define DOCTEST_CONFIG_NO_EXCEPTIONS

#include "control_server_test.h"

MocTest::MocTest() : cs(new ControlServer(this)) {
  connect(cs, &ControlServer::recieveFileInfo, this, &MocTest::recieveFileInfo);
}
MocTest::~MocTest() {}

void MocTest::run() {
  QStringList files{"test1.txt", "test2"};  // 文件路径

  // 检查文件是否存在
  for (auto&& i : files) {
    QFile file(i);
    if (!file.exists()) {
      // 文件不存在，创建文件
      if (file.open(QIODevice::ReadWrite)) {
        // 可以写入文件内容或执行其他操作
        file.close();
      }
    }
  }

  cs->onSendFile(files, QHostAddress("127.0.0.1"));
}

void MocTest::acceptSendFile(const QVector<FileInfo>& filenames,
                             const QHostAddress& dst) {}
void MocTest::denySendFile(const QVector<FileInfo>& filenames,
                           const QHostAddress& dst) {}
void MocTest::recieveFileInfo(const QVector<FileInfo>& filenames,
                              const QHostAddress& dst) {
  CHECK(true);
}
void MocTest::cancelFileInfo(const QVector<FileInfo>& filenames,
                             const QHostAddress& dst) {}

TEST_CASE("control server send file info and recieve") {
  MocTest test;
  test.run();
}

// int main(int argc, char** argv) {
//   doctest::Context context;
//   int result = context.run();
//   return result;
// }