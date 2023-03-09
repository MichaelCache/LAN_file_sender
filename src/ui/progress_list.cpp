#include "progress_list.h"

#include <QHeaderView>

enum class Column : int { DestIP = 0, FileName, FileSize, State, Progress };

ProgressListView::ProgressListView(QWidget *parent) : QTableView(parent) {
  setContextMenuPolicy(Qt::CustomContextMenu);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setWordWrap(false);
  horizontalHeader()->setStretchLastSection(true);
  verticalHeader()->setVisible(false);

  setColumnWidth((int)Column::FileName, 340);
  setColumnWidth((int)Column::Progress, 160);
}

ProgressListView::~ProgressListView() {}