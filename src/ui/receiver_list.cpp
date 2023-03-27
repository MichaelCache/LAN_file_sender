#include "receiver_list.h"

#include <QContextMenuEvent>
#include <QHeaderView>

ReceiverListView::ReceiverListView(QWidget *parent) : QTableView(parent) {
  setSelectionMode(QAbstractItemView::MultiSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setWordWrap(false);
  horizontalHeader()->setStretchLastSection(true);
  horizontalHeader()->setSelectionMode(QAbstractItemView::NoSelection);
  horizontalHeader()->setHighlightSections(false);
  verticalHeader()->setVisible(false);

  setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  connect(this, &ReceiverListView::customContextMenuRequested, this,
          &ReceiverListView::onReceiverContextMenuRequested);

  m_file_dialog = new QFileDialog(this);

  m_right_menu = new QMenu(this);
  m_send_ac = new QAction("Send", this);
  connect(m_send_ac, &QAction::triggered, m_file_dialog, &QFileDialog::show);
  m_right_menu->addAction(m_send_ac);
}

ReceiverListView::~ReceiverListView() {}

void ReceiverListView::onReceiverContextMenuRequested(const QPoint &pos) {
  QModelIndex index = indexAt(pos);
  if (index.isValid()) {
    QPoint glob_pos = mapToGlobal(pos);
    m_right_menu->exec(glob_pos);
  }
}

void ReceiverListView::onSendFile() {
  // QModelIndex currIndex = currentIndex();
  // QModelIndex fileNameIndex = model()->index(
  //     currIndex.row(), (int)TransferTableModel::Column::FileName);
  // QString fileName = mSenderModel->data(fileNameIndex).toString();

  // QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}