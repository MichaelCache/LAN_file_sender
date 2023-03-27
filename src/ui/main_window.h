#pragma once

#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>

#include "main_widget.h"

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private Q_SLOTS:
  void openSettingDialog();

 protected:
  virtual void closeEvent(QCloseEvent *event) override;

 private:
  MainWidget *m_central_widget;
  QMenuBar *m_menubar;
  QAction *m_setting;
  QMessageBox *m_close_msg;
};
