#include "tcp_package.h"

QByteArray TcpPackage::packData(PackageType type, const QByteArray& data) {
  QByteArray result;
  PackageSize size = sizeof(PackageSize) + sizeof(PackageType) + data.size();
  result.append((char*)(&size), sizeof(PackageSize));
  result.append((char*)(&type), sizeof(PackageType));
  result.append(data);
  return result;
}

TcpPackage TcpPackage::unpackData(const QByteArray& data) {
  PackageSize size{0};
  memcpy(&size, data.data(), sizeof(PackageSize));

  if (size > data.size()) {
    // buff not cotains completely package, wait for more data
    return {PackageType::None, 0, QByteArray()};
  }

  auto pack_data = data.mid(0, size);
  pack_data.remove(0, sizeof(PackageSize));
  PackageType type;
  memcpy(&type, pack_data.data(), sizeof(PackageType));
  pack_data.remove(0, sizeof(PackageType));
  return {type, size, pack_data};
}