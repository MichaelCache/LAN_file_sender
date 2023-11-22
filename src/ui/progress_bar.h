#pragma once
#include <QStyledItemDelegate>
class ProgressBar : public QStyledItemDelegate {
  Q_OBJECT
 public:
  using QStyledItemDelegate::QStyledItemDelegate;
  // ProgressBar(QObject *parent = nullptr);
  ~ProgressBar() = default;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;
};
