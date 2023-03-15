#pragma once

#include <QDialog>
#include <QPushButton>

class SettingDialog : public QDialog {
  Q_OBJECT
 public:
  SettingDialog(QWidget* parent = nullptr);
  ~SettingDialog();

 private:
  void loadSetting();

  QPushButton* m_sure_bt;
  QPushButton* m_cancel_bt;

 private Q_SLOTS:
  void saveSetting();
};