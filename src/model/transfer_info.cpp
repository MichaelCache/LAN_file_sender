#include "transfer_info.h"
TransferInfo::TransferInfo(/* args */) {
  m_id = reinterpret_cast<qintptr>(this);
}

TransferInfo::TransferInfo(const QString& type, const QHostAddress& dst,
                           const QString& filename, quint64 size,
                           TransferState state, quint8 progress)
    : m_type(type),
      m_dest_ip(dst),
      m_file_name(filename),
      m_file_size(size),
      m_state(state),
      m_progress(progress) {
  m_id = reinterpret_cast<qintptr>(this);
}

TransferInfo::~TransferInfo() {}

bool TransferInfo::operator==(const TransferInfo& other) const {
  // stats and progress is variable;
  return m_id == other.m_id;
}

qintptr TransferInfo::id() const { return m_id; }
