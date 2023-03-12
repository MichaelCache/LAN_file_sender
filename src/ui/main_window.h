#pragma once

#include <QMainWindow>

#include "main_widget.h"

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private Q_SLOTS:
  void openSettingDialog();

 private:
  MainWidget *m_central_widget;
  QMenuBar *m_menubar;
  QAction *m_setting;
};
