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

enum class TransferType { Upload, Download };

QString transferTypeToString(TransferType type);

class TransferInfo {
 private:
  /* data */
 public:
  TransferInfo(/* args */);
  ~TransferInfo();

  bool operator==(const TransferInfo&) const;

  QUuid id() const;

  TransferType m_type;  // TransferState in string
  QHostAddress m_dest_ip;
  QString m_file_path;
  QString m_file_name;
  quint64 m_file_size{0};  // in byte
  TransferState m_state{TransferState::Waiting};
  quint8 m_progress{0};
  QUuid m_id;
};
