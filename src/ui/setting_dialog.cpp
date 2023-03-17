#include "setting_dialog.h"

SettingDialog::SettingDialog(QWidget *parent) : QDialog(parent) {
  resize(450, 470);
  setMinimumSize(QSize(450, 470));
  setMaximumSize(QSize(450, 470));
  setWindowTitle("Setting");

  m_layout = new QVBoxLayout(this);

  m_content_layout = new QGridLayout();
  m_buttons_layout = new QHBoxLayout();

  m_reset_bt = new QPushButton("Reset", this);
  m_sure_bt = new QPushButton("Save", this);
  m_cancel_bt = new QPushButton("Cancel", this);
  m_cancel_bt->setDefault(true);

  m_buttons_layout->addWidget(m_reset_bt);
  m_buttons_layout->addStretch();
  m_buttons_layout->addWidget(m_sure_bt);
  m_buttons_layout->addWidget(m_cancel_bt);

  m_layout->addLayout(m_content_layout);
  m_layout->addStretch();
  m_layout->addLayout(m_buttons_layout);

  connect(m_sure_bt, &QPushButton::clicked, this, &SettingDialog::saveSetting);
  connect(m_cancel_bt, &QPushButton::clicked, this, &SettingDialog::reject);

  // (m_layout);
}

SettingDialog::~SettingDialog() {}

void SettingDialog::loadSetting() {}

void SettingDialog::saveSetting() {}
