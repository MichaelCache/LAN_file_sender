#include "main_window.h"

#include <QAction>
#include <QMenuBar>
#include <QMessageBox>

#include "config.h"
#include "server/dhcp_server.h"
#include "setting_dialog.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  m_central_widget = new MainWidget(this);
  setCentralWidget(m_central_widget);

  m_menubar = new QMenuBar(this);
  m_setting = new QAction("Setting", m_menubar);
  m_menubar->addAction(m_setting);

  connect(m_setting, &QAction::triggered, this, &MainWindow::openSettingDialog);

  setMenuBar(m_menubar);

  // close dialog
  m_close_msg = new QMessageBox(this);
  m_close_msg->setText("Are you sure?");
  m_close_msg->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  m_close_msg->setDefaultButton(QMessageBox::No);
}

MainWindow::~MainWindow() {}

void MainWindow::openSettingDialog() {
  SettingDialog dialog;
  dialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  auto ret = m_close_msg->exec();
  if (ret != QMessageBox::Yes) {
    event->ignore();
#ifdef Q_OS_WIN
    dhcpServerStop();
#else
#endif
  } else {
    m_central_widget->onClose();
    event->accept();
  }
}