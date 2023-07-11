#include "receiver_list.h"

#include <QContextMenuEvent>
#include <QHeaderView>

#include "model/column.h"
#include "server/transfer_info.h"

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

  m_right_menu = new QMenu(this);
  m_send_ac = new QAction("Send", this);
  m_right_menu->addAction(m_send_ac);
  connect(m_send_ac, &QAction::triggered, this,
          &ReceiverListView::openFileDialog);
}

ReceiverListView::~ReceiverListView() {}

void ReceiverListView::onReceiverContextMenuRequested(const QPoint &pos) {
  QModelIndex index = indexAt(pos);
  if (index.isValid()) {
    QPoint glob_pos = mapToGlobal(pos);
    m_right_menu->exec(glob_pos);
  }
}

void ReceiverListView::onSendFile(const QStringList &filenames) {
  QModelIndex curr_index = currentIndex();
  QModelIndex ip_index =
      model()->index(curr_index.row(), (int)RemoteClient::Column::IP);
  auto dst = QHostAddress(model()->data(ip_index).toString());
  QVector<TransferInfo> infos;
  for (auto &&f : filenames) {
    TransferInfo info;
    info.m_dest_ip = dst;
    info.m_file_path = f;
    info.m_file_name = f;
    info.m_file_size = 0;
    infos.append(info);
  }
  emit sendFile(inofs);
}

void ReceiverListView::openFileDialog() {
  QFileDialog file_dialog(this);
  file_dialog.setFileMode(QFileDialog::ExistingFiles);
  file_dialog.setAttribute(Qt::WA_DeleteOnClose, true);
  if (file_dialog.exec()) {
    QStringList selected_files = file_dialog.selectedFiles();
    onSendFile(selected_files);
  }
}
