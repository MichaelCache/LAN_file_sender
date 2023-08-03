#include "transfer_info.h"

#include <QFileInfo>
#include <QMetaObject>

TransferInfo::TransferInfo(/* args */) { qRegisterMetaType<TransferInfo>(); }

TransferInfo::TransferInfo(const FileInfo& info) {
  m_file_name = info.m_filename;
  m_file_size = info.m_byte;
  m_id = info.m_id;
}

TransferInfo::~TransferInfo() {}

bool TransferInfo::operator==(const TransferInfo& other) const {
  // stats and progress is variant, only match taskid;
  return m_id == other.m_id;
}

QUuid TransferInfo::id() const { return m_id; }
