#pragma once

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>

class SettingDialog : public QDialog {
  Q_OBJECT
 public:
  SettingDialog(QWidget* parent = nullptr);
  ~SettingDialog();

 private:
  void loadSetting();

  QVBoxLayout* m_layout;

  QGridLayout* m_content_layout;
  QSpacerItem* m_content_space;
  
  QHBoxLayout* m_buttons_layout;
  QSpacerItem* m_bt_space;

  QPushButton* m_reset_bt;
  QPushButton* m_sure_bt;
  QPushButton* m_cancel_bt;

 private Q_SLOTS:
  void saveSetting();
};