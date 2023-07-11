#pragma once

#include <QHostAddress>
#include <QString>
#include <QUuid>

#include "fileinfo.h"

enum class TransferState : quint32 {
  UnKonwn = 0,
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
  TransferInfo(const FileInfo& info);
  ~TransferInfo();

  using TaskId = QUuid;

  bool operator==(const TransferInfo&) const;

  TaskId id() const;

  // TransferType m_type;  // TransferState in string
  QHostAddress m_from_ip;
  QHostAddress m_dest_ip;
  QString m_file_path;     // filename with path
  QString m_file_name;     // short filename name
  quint64 m_file_size{0};  // in byte
  TransferState m_state{TransferState::UnKonwn};
  quint8 m_progress{0};

 private:
  TaskId m_id{QUuid::createUuid()};
};
