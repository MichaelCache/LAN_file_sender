#pragma once

#include <QDataStream>
#include <QString>
#include <QUuid>
#include <QVector>

struct FileInfo {
  FileInfo(const QString &filename = "", qint64 size = 0);
  QString m_fullname;
  qint64 m_byte;
  QUuid m_id{QUuid::createUuid()};
};

QVector<FileInfo> fileListToFileInfo(const QStringList &);

QDataStream &operator<<(QDataStream &out, const FileInfo &info);

QDataStream &operator>>(QDataStream &in, FileInfo &info);

QDataStream &operator<<(QDataStream &out, const QVector<FileInfo> &infos);

QDataStream &operator>>(QDataStream &in, QVector<FileInfo> &infos);
