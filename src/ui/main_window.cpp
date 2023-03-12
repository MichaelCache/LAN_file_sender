#include "main_window.h"

#include <QAction>
#include <QMenuBar>

#include "setting_dialog.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  m_central_widget = new MainWidget(this);
  setCentralWidget(m_central_widget);

  m_menubar = new QMenuBar(this);
  m_setting = new QAction("Setting", m_menubar);
  m_menubar->addAction(m_setting);

  connect(m_setting, &QAction::triggered, this, &MainWindow::openSettingDialog);

  setMenuBar(m_menubar);
}

MainWindow::~MainWindow() {}

void MainWindow::openSettingDialog() {
  SettingDialog dialog;
  dialog.exec();
}