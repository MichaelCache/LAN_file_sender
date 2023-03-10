#include "progress_list.h"

#include <QHeaderView>

#include "model/column.h"

ProgressListView::ProgressListView(QWidget *parent) : QTableView(parent) {
  //   setContextMenuPolicy(Qt::CustomContextMenu);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setWordWrap(false);
  horizontalHeader()->setStretchLastSection(true);
  verticalHeader()->setVisible(false);

  setColumnWidth((int)ProgressColumn::FileName, 340);
  setColumnWidth((int)ProgressColumn::Progress, 160);
}

ProgressListView::~ProgressListView() {}