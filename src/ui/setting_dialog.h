#pragma once

#include <QDialog>

class SettingDialog : public QDialog {
  Q_OBJECT

 public:
  SettingDialog(QWidget *parent = nullptr);
  ~SettingDialog();

 private:
  void loadSetting();

 private Q_SLOTS:
  void saveSetting();
};