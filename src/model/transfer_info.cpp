#include "transfer_info.h"
TransferInfo::TransferInfo(/* args */) {}

TransferInfo::TransferInfo(const QString& type, const QHostAddress& dst,
                           const QString& filename, quint64 size,
                           TransferState state, quint8 progress)
    : m_type(type),
      m_dest_ip(dst),
      m_file_name(filename),
      m_file_size(size),
      m_state(state),
      m_progress(progress) {}

TransferInfo::~TransferInfo() {}

bool TransferInfo::operator==(const TransferInfo& other) {
  // stats and progress is variable;
  return m_type == other.m_type && m_dest_ip == other.m_dest_ip &&
         m_file_name == other.m_file_name && m_file_size == other.m_file_size;
}
