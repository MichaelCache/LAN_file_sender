#pragma once

namespace RemoteClient {
enum class Column : int { IP = 0, Name, OS, Count };
}  // namespace RemoteClient

namespace TransferProgress {
enum class Column : int {
  Type = 0,
  DestIP,
  FileName,
  FileSize,
  State,
  Progress,
  Count
};
}
