#include "transfer_info.h"

QString transferTypeToString(TransferType type) {
  switch (type) {
    case TransferType::Upload:
      return "Upload";
      break;
    case TransferType::Download:
      return "Download";
      break;
    default:
      return "";
      break;
  }
}

TransferInfo::TransferInfo(/* args */) {}

TransferInfo::~TransferInfo() {}

bool TransferInfo::operator==(const TransferInfo& other) const {
  // stats and progress is variable;
  return m_id == other.m_id;
}

QUuid TransferInfo::id() const { return m_id; }
