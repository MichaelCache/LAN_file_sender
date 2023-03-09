#include "main_window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  m_central_widget = new MainWidget(this);
  setCentralWidget(m_central_widget);
}

MainWindow::~MainWindow() {}
