#pragma once

namespace RemoteClient {
enum class Column : int { IP = 0, Name, OS, Count };
}  // namespace RemoteServer

namespace TransferProgress {
enum class Column : int {
  DestIP = 0,
  FileName,
  FileSize,
  State,
  Progress,
  Count
};
}
