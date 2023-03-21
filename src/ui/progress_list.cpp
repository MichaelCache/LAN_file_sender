#include "progress_list.h"

#include <QHeaderView>

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
}

ProgressListView::~ProgressListView() {}