#include "progress_bar.h"
#include "model/column.h"

#include <QPainter>
#include <QApplication>

ProgressBar::ProgressBar(QObject *parent) : QStyledItemDelegate(parent) {}

ProgressBar::~ProgressBar() {}

void ProgressBar::paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const {
  if (index.column() == (int)TransferProgress::Column::Progress) {
    int value = index.model()->data(index).toInt();
    QStyleOptionProgressBar progressBarOption;
    progressBarOption.rect = option.rect.adjusted(4, 4, -4, -4);
    progressBarOption.minimum = 0;
    progressBarOption.maximum = 100;
    progressBarOption.textAlignment = Qt::AlignHCenter;
    progressBarOption.textVisible = true;
    progressBarOption.progress = value;
    progressBarOption.text = QString("%1%").arg(progressBarOption.progress);

    painter->save();
    if (option.state & QStyle::State_Selected) {
      painter->fillRect(option.rect, option.palette.highlight());
      painter->setBrush(option.palette.highlightedText());
    }
    QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                       &progressBarOption, painter);

    painter->restore();

  } else {
    return;
  }
}