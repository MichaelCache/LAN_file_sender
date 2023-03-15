#include "setting.h"

#include "config.h"

Setting::Setting(const QString &organization, QObject *parent)
    : QSettings(organization, APP_NAME, parent) {}

Setting::~Setting() {}

Setting &Setting::ins() {
  static Setting s;
  return s;
}
