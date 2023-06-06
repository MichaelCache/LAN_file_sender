#pragma once

#include <QDataStream>
#include <QString>
#include <QVector>

enum class ControlSignal : int {
  None = 0,
  SendInfo,
  CancelSend,
  Finish,
};

struct FileInfo {
  QString m_name;
  qint64 m_byte;
};

QDataStream &operator<<(QDataStream &out, const FileInfo &info);

QDataStream &operator>>(QDataStream &in, FileInfo &info);

QDataStream &operator<<(QDataStream &out, const QVector<FileInfo> &infos);

QDataStream &operator>>(QDataStream &in, QVector<FileInfo> &infos);
