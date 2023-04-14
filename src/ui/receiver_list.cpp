#include "receiver_list.h"

#include <QContextMenuEvent>
#include <QHeaderView>

#include "model/column.h"

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

  // set column width
  setColumnWidth(3, 20);

  // right click menu
  setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  connect(this, &ReceiverListView::customContextMenuRequested, this,
          &ReceiverListView::onReceiverContextMenuRequested);

  m_file_dialog = new QFileDialog(this);
  m_file_dialog->setFileMode(QFileDialog::ExistingFiles);
  connect(m_file_dialog, &QFileDialog::filesSelected, this,
          &ReceiverListView::onSendFile);

  m_right_menu = new QMenu(this);
  m_send_ac = new QAction("Send", this);
  m_right_menu->addAction(m_send_ac);
  connect(m_send_ac, &QAction::triggered, m_file_dialog, &QFileDialog::show);
}

ReceiverListView::~ReceiverListView() {}

void ReceiverListView::onReceiverContextMenuRequested(const QPoint &pos) {
  QModelIndex index = indexAt(pos);
  if (index.isValid()) {
    QPoint glob_pos = mapToGlobal(pos);
    m_right_menu->exec(glob_pos);
  }
  // clear history
  m_file_dialog->setHistory(QStringList());
}

void ReceiverListView::onSendFile(const QStringList &filenames) {
  QModelIndex curr_index = currentIndex();
  QModelIndex ip_index =
      model()->index(curr_index.row(), (int)RemoteClient::Column::IP);
  auto dst = QHostAddress(model()->data(ip_index).toString());
  for (auto &&f : filenames) {
    emit sendFile(f, dst);
  }
}