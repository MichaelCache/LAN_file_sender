#pragma once

#include <QDataStream>
#include <QString>
#include <QUuid>
#include <QVector>

/**
 * @brief short info message for TransferInfo
 *
 */
struct FileInfo {
  FileInfo(const QString &filename = "", quint64 size = 0,
           QUuid id = QUuid::createUuid());
  QString m_filename;
  quint64 m_byte;
  QUuid m_id;
};

QVector<FileInfo> fileListToFileInfo(const QStringList &);

QDataStream &operator<<(QDataStream &out, const FileInfo &info);

QDataStream &operator>>(QDataStream &in, FileInfo &info);

QDataStream &operator<<(QDataStream &out, const QVector<FileInfo> &infos);

QDataStream &operator>>(QDataStream &in, QVector<FileInfo> &infos);
