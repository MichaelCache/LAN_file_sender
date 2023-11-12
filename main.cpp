#include <QApplication>

#include "ui/main_window.h"

int main(int argc, char *argv[]) {
  // enable high dpi
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);

  MainWindow w;
  w.show();

  return app.exec();
}
