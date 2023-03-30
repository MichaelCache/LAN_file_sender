#include "tcp_package.h"

TcpPackage::TcpPackage(/* args */) {}

TcpPackage::~TcpPackage() {}

QByteArray TcpPackage::preparePackage(PackageType type, QByteArray data) {
  QByteArray result;
  PackageSize size = sizeof(PackageSize) + sizeof(PackageType) + data.size();
  result.append((char*)(&size), sizeof(PackageSize));
  result.append((char*)(&type), sizeof(PackageType));
  result.append(data);
  return result;
}