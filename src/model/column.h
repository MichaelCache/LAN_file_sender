#pragma once

enum class ReceiverColumn : int { DestIP = 0, Comment };

enum class ProgressColumn : int {
  DestIP = 0,
  FileName,
  FileSize,
  State,
  Progress
};