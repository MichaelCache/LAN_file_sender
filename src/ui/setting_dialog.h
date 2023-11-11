#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QSpinBox>

class SettingDialog : public QDialog {
  Q_OBJECT
 public:
  SettingDialog(QWidget* parent = nullptr);
  ~SettingDialog() = default;

 public Q_SLOTS:
  virtual int exec() override;

 private Q_SLOTS:
  void saveSetting();
  void selectDownloadDir();
  void resetSetting();

 private:
  void getSetting();

  QVBoxLayout* m_layout;

  QGridLayout* m_content_layout;
  QGroupBox* m_group_box;
  QLabel* m_hostname_label;
  QHBoxLayout* m_dir_layout;
  QLineEdit* m_hostname_edit;
  QLabel* m_download_dir_label;
  QLineEdit* m_download_dir_edit;
  QPushButton* m_select_dir_bt;
  QLabel* m_broad_interval_label;
  QLabel* m_broad_interval_unit;
  QSpinBox* m_broad_interval_edit;

  QHBoxLayout* m_buttons_layout;
  QPushButton* m_reset_bt;
  QPushButton* m_sure_bt;
  QPushButton* m_cancel_bt;
};