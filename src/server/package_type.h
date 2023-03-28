#pragma once

enum class PackageType : char {
  Header = 0x01,
  Data,
  Finish,
  Cancel,
  Pause,
  Resume
};

using PackageSize = uint32_t;