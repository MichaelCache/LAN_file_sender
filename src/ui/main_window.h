#pragma once

#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>

#include "main_widget.h"
#include "setting_dialog.h"

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow() = default;

 protected:
  virtual void closeEvent(QCloseEvent *event) override;

 private:
  MainWidget *m_central_widget;
  QMenuBar *m_menubar;
  QAction *m_setting_ac;
  QMessageBox *m_close_msg;
  SettingDialog *m_setting_dialog;
};
