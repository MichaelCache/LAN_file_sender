#pragma once

#include <QHostAddress>
#include <QString>

enum class TransferState {
  Idle,
  Waiting,
  Disconnected,
  Paused,
  Cancelled,
  Transfering,
  Finish
};

class TransferInfo {
 private:
  /* data */
 public:
  TransferInfo(/* args */);
  ~TransferInfo();

  QHostAddress m_dest_ip;
  QString m_file_name;
  quint64 m_file_size{0};
  TransferState m_state{TransferState::Idle};
  quint8 m_progress{0};
};