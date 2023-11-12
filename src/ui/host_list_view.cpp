#include "host_list.h"

#include <QContextMenuEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QNetworkInterface>

#include "model/host_interface.h"
#include "server/transfer_info.h"

HostListView::HostListView(QWidget *parent) : QTableView(parent) {
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
  connect(this, &HostListView::customContextMenuRequested, this,
          &HostListView::onReceiverContextMenuRequested);

  m_right_menu = new QMenu(this);
  m_send_ac = new QAction("Send", this);
  m_right_menu->addAction(m_send_ac);
  connect(m_send_ac, &QAction::triggered, this, &HostListView::openFileDialog);
}

HostListView::~HostListView() {}

void HostListView::onReceiverContextMenuRequested(const QPoint &pos) {
  QModelIndex index = indexAt(pos);
  if (index.isValid()) {
    QPoint glob_pos = mapToGlobal(pos);
    m_right_menu->exec(glob_pos);
  }
}

void HostListView::onSendFile(const QStringList &filenames) {
  QModelIndex curr_index = currentIndex();
  QModelIndex ip_index =
      model()->index(curr_index.row(), (int)RemoteClient::Column::IP);
  auto dst = QHostAddress(model()->data(ip_index).toString());

  QVector<TransferInfo> infos;
  for (auto &&f : filenames) {
    TransferInfo info;
    info.m_dest_ip = dst;
    // no need set from ip, reciever will get ip from socket
    info.m_from_ip = info.m_file_path = f;
    QFileInfo file_info(f);
    info.m_file_name = file_info.fileName();
    info.m_file_size = file_info.size();
    info.m_state = TransferState::Waiting;
    infos.append(info);
  }
  emit sendFile(infos);
}

void HostListView::openFileDialog() {
  QFileDialog file_dialog;
  file_dialog.setFileMode(QFileDialog::ExistingFiles);
  if (file_dialog.exec()) {
    QStringList selected_files = file_dialog.selectedFiles();
    onSendFile(selected_files);
  }
}
