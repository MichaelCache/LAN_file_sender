#pragma once


#include <QMainWindow>



#include "main_widget.h"


class MainWindow : public QMainWindow {
 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private:
  MainWidget *m_central_widget;
  
};
