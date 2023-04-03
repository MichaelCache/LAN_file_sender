#pragma once

#include <QByteArray>

#include "package_type.h"

class TcpPackage {
 private:
  /* data */
 public:
  TcpPackage(/* args */);
  ~TcpPackage();

 protected:
  QByteArray preparePackage(PackageType type, QByteArray data = QByteArray());
};
