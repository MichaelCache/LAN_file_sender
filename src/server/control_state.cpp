#include "control_state.h"

#include <QFileInfo>

QVector<FileInfo> fileListToFileInfo(const QStringList &filenames) {
  QVector<FileInfo> infos;
  for (auto &&i : filenames) {
    QFileInfo fileinfo(i);
    FileInfo info{i, fileinfo.size()};
    infos.push_back(info);
  }
  return infos;
}

QDataStream &operator<<(QDataStream &out, const FileInfo &info) {
  out << info.m_name << info.m_byte << info.m_id;
  return out;
}

QDataStream &operator>>(QDataStream &in, FileInfo &info) {
  in >> info.m_name >> info.m_byte >> info.m_id;
  return in;
}

QDataStream &operator<<(QDataStream &out, const QVector<FileInfo> &infos) {
  out << infos.size();
  for (auto &&i : infos) {
    out << i;
  }
  return out;
}

QDataStream &operator>>(QDataStream &in, QVector<FileInfo> &infos) {
  int size = 0;
  in >> size;
  for (size_t i = 0; i < size; i++) {
    FileInfo info;
    in >> info;
    infos.push_back(info);
  }
  return in;
}
