#include "progress_list.h"

#include <QHeaderView>

#include "model/column.h"

using TransferProgress::Column;

ProgressListView::ProgressListView(QWidget *parent) : QTableView(parent) {
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setWordWrap(false);
  horizontalHeader()->setStretchLastSection(true);
  horizontalHeader()->setSelectionMode(QAbstractItemView::NoSelection);
  horizontalHeader()->setHighlightSections(false);
  verticalHeader()->setVisible(false);

  m_progress = new ProgressBar(this);
  // m_progress->setAutoFillBackground(true);
  setItemDelegateForColumn((int)Column::Progress, m_progress);
}

ProgressListView::~ProgressListView() {}