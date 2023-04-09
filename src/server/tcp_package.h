#pragma once

#include <QByteArray>
#include <QPair>

enum class PackageType : char {
  None = 0x00,
  Header,
  Data,
  Finish,
  Cancel,
  Pause,
  Resume
};

using PackageSize = int;

struct TcpPackage {
 public:
  static QByteArray packData(PackageType type,
                             const QByteArray& data = QByteArray());
  static TcpPackage unpackData(const QByteArray& data);

  PackageType m_type;
  PackageSize m_size;
  QByteArray m_data;
};
