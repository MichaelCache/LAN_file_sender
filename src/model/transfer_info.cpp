#include "transfer_info.h"

TransferInfo::TransferInfo(/* args */) {}

TransferInfo::~TransferInfo() {}

bool TransferInfo::operator==(const TransferInfo& other) const {
  // stats and progress is variable;
  return m_id == other.m_id;
}

QUuid TransferInfo::id() const { return m_id; }
