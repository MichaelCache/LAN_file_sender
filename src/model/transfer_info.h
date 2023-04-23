#pragma once

#include <QHostAddress>
#include <QString>
#include <QUuid>

enum class TransferState {
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

  bool operator==(const TransferInfo&) const;

  QUuid id() const;

  QString m_type;
  QHostAddress m_dest_ip;
  QString m_file_path;
  QString m_file_name;
  quint64 m_file_size{0};
  TransferState m_state{TransferState::Waiting};
  quint8 m_progress{0};

 private:
  QUuid m_id{QUuid::createUuid()};
};