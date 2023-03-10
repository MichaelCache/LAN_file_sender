#include "receiver_list.h"
#include "model/column.h"

ReceiverListView::ReceiverListView(QWidget *parent) : QTableView(parent) {
  setColumnWidth((int)ReceiverColumn::DestIP, 600);
  setColumnWidth((int)ReceiverColumn::Comment, 300);
}

ReceiverListView::~ReceiverListView() {}