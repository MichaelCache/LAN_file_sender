#include "progress_bar.h"

ProgressBar::ProgressBar(QObject *parent) : QStyledItemDelegate(parent) {}

ProgressBar::~ProgressBar() {}

void ProgressBar::paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const {}