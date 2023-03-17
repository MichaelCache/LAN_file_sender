#pragma once

enum class ReceiverColumn : int { DestIP = 0, Name, OS };

enum class ProgressColumn : int {
  DestIP = 0,
  FileName,
  FileSize,
  State,
  Progress
};