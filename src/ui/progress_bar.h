#pragma once
#include <QStyledItemDelegate>
class ProgressBar : public QStyledItemDelegate {
  Q_OBJECT
 public:
  ProgressBar(QObject *parent = nullptr);
  ~ProgressBar();

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;
};
