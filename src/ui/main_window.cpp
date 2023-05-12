#include "main_window.h"

#include <QAction>
#include <QMenuBar>
#include <QMessageBox>

#include "config.h"
#include "setting.h"
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
  m_close_msg->setText("File in transfering will lost. Are you sure exit?");
  m_close_msg->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  m_close_msg->setDefaultButton(QMessageBox::No);

  // setting dialog
  m_setting_dialog = new SettingDialog(this);
  connect(&Setting::ins(), &Setting::updateSettings, m_setting_dialog,
          &SettingDialog::onUpdataSettings);
}

MainWindow::~MainWindow() {}

void MainWindow::openSettingDialog() { m_setting_dialog->exec(); }

void MainWindow::closeEvent(QCloseEvent *event) {
  auto ret = m_close_msg->exec();
  if (ret != QMessageBox::Yes) {
    event->ignore();

  } else {
    m_central_widget->onClose();
    event->accept();
  }
}