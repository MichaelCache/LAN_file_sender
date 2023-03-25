#include "main_window.h"

#include <QAction>
#include <QMenuBar>
#include <QMessageBox>

#include "config.h"
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

void MainWindow::closeEvent(QCloseEvent *event) {
  QMessageBox::StandardButton resBtn = QMessageBox::question(
      this, APP_NAME, "Are you sure?", QMessageBox::Yes | QMessageBox::No,
      QMessageBox::No);
  if (resBtn != QMessageBox::Yes) {
    event->ignore();
  } else {
    m_central_widget->onClose();
    event->accept();
  }
}