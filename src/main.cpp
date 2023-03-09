#include <QApplication>

#include "ui/main_window.h"

int main(int argc, char *argv[]) {
  // enable high dpi
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);

  // wait finish file transfer even window closed;
  // app.setQuitOnLastWindowClosed(false);

  MainWindow w;
  w.resize(900, 500);
  w.show();

  return app.exec();
}
