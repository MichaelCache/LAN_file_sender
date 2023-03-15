#include "setting_dialog.h"

SettingDialog::SettingDialog(QWidget *parent) : QDialog(parent) {
  m_sure_bt = new QPushButton(this);
  connect(m_sure_bt, &QPushButton::clicked, this, &SettingDialog::saveSetting);
}

SettingDialog::~SettingDialog() {}

void SettingDialog::loadSetting() {}

void SettingDialog::saveSetting() {}
