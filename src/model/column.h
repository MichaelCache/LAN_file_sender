#pragma once

#include <QtCore>

namespace RemoteClient {
enum class Column : int { IP = 0, Name, OS, Count };
}

namespace TransferProgress {
enum class Column : int {
  Type = 0,
  DestIP,
  FileName,
  FileSize,
  State,
  Progress,
  Count,
  // id won't be shown in ui
  Id
};
}

enum MyRole { IdRole = Qt::UserRole, PathRole };
