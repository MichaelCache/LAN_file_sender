#include "setting_dialog.h"

#include <QFileDialog>

#include "setting.h"

SettingDialog::SettingDialog(QWidget* parent) : QDialog(parent) {
  resize(450, 470);
  setMinimumSize(QSize(450, 470));
  setMaximumSize(QSize(450, 470));
  setWindowTitle("Setting");

  m_layout = new QVBoxLayout(this);

  m_buttons_layout = new QHBoxLayout();

  // content
  m_group_box = new QGroupBox(this);
  m_group_box->setTitle("Profile");

  // hostname
  m_hostname_label = new QLabel("HostName", m_group_box);
  m_hostname_edit = new QLineEdit(Setting::ins().m_hostname, m_group_box);
  // download dir
  m_download_dir_label = new QLabel("Download", m_group_box);
  m_download_dir_edit =
      new QLineEdit(Setting::ins().m_download_dir, m_group_box);
  m_select_dir_bt = new QPushButton("Browse", m_group_box);
  connect(m_select_dir_bt, &QPushButton::clicked, this,
          &SettingDialog::selectDownloadDir);

  m_dir_layout = new QHBoxLayout();
  m_dir_layout->addWidget(m_download_dir_edit);
  m_dir_layout->addWidget(m_select_dir_bt);

// broadcast interval
  m_broad_interval_label = new QLabel("Broadcast Interval", m_group_box);
  m_broad_interval_edit = new QSpinBox(m_group_box);
  m_broad_interval_edit->setRange(0, 9999);
  m_broad_interval_edit->setValue(Setting::ins().m_boradcast_interval);
  m_broad_interval_unit = new QLabel("ms", m_group_box);

  // content layout
  m_content_layout = new QGridLayout(m_group_box);
  m_content_layout->addWidget(m_hostname_label, 0, 0, 1, 1);
  m_content_layout->addWidget(m_hostname_edit, 0, 1, 1, 3);
  m_content_layout->addWidget(m_download_dir_label, 1, 0, 1, 1);
  m_content_layout->addLayout(m_dir_layout, 1, 1);
  m_content_layout->addWidget(m_broad_interval_label, 2, 0, 1, 1);
  m_content_layout->addWidget(m_broad_interval_edit, 2, 1, 1, 1);
  m_content_layout->addWidget(m_broad_interval_unit, 2, 2, 1, 1);

  // button of bottom
  m_reset_bt = new QPushButton("Reset", this);
  m_sure_bt = new QPushButton("Save", this);
  m_cancel_bt = new QPushButton("Cancel", this);
  m_cancel_bt->setDefault(true);

  m_buttons_layout->addWidget(m_reset_bt);
  m_buttons_layout->addStretch();
  m_buttons_layout->addWidget(m_sure_bt);
  m_buttons_layout->addWidget(m_cancel_bt);

  // main layout
  m_layout->addWidget(m_group_box);
  m_layout->addStretch();
  m_layout->addLayout(m_buttons_layout);

  connect(m_sure_bt, &QPushButton::clicked, this, &SettingDialog::saveSetting);
  connect(m_cancel_bt, &QPushButton::clicked, this, &SettingDialog::reject);
  connect(m_reset_bt, &QPushButton::clicked, this,
          &SettingDialog::resetSetting);

  // (m_layout);
}

SettingDialog::~SettingDialog() {}

void SettingDialog::onUpdataSettings() {
  auto& s = Setting::ins();
  m_hostname_edit->setText(s.m_hostname);
  m_download_dir_edit->setText(s.m_download_dir);
  m_broad_interval_edit->setValue(s.m_boradcast_interval);
}

void SettingDialog::saveSetting() {
  auto& s = Setting::ins();
  s.m_download_dir = m_download_dir_edit->text();
  s.m_hostname = m_hostname_edit->text();
  s.m_boradcast_interval = m_broad_interval_edit->value();
  s.saveSettings();
  accept();
}

void SettingDialog::selectDownloadDir() {
  QString download_dir = Setting::ins().m_download_dir;
  QString new_dir = QFileDialog::getExistingDirectory(
      this, "Select a directory", download_dir);

  if (!new_dir.isEmpty()) m_download_dir_edit->setText(new_dir);
}

void SettingDialog::resetSetting() { Setting::ins().reset(); }
