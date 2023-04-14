#include "progress_list.h"

#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QHeaderView>

#include "model/column.h"
#include "model/progress_model.h"
#include "setting.h"

using TransferProgress::Column;

ProgressListView::ProgressListView(QWidget *parent) : QTableView(parent) {
  setSelectionMode(QAbstractItemView::MultiSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  setWordWrap(false);
  horizontalHeader()->setStretchLastSection(true);
  horizontalHeader()->setSelectionMode(QAbstractItemView::NoSelection);
  horizontalHeader()->setHighlightSections(false);
  verticalHeader()->setVisible(false);

  m_progress = new ProgressBar(this);
  setItemDelegateForColumn((int)Column::Progress, m_progress);

  // right click menu
  setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  connect(this, &ProgressListView::customContextMenuRequested, this,
          &ProgressListView::onReceiverContextMenuRequested);

  m_right_menu = new QMenu(this);
  m_cancel_ac = new QAction("Cancel", this);
  m_open_dir_ac = new QAction("Open In Dir", this);
  m_clear_ac = new QAction("Clear Finished", this);
  m_right_menu->addAction(m_cancel_ac);
  m_right_menu->addAction(m_open_dir_ac);
  m_right_menu->addAction(m_clear_ac);
  connect(m_cancel_ac, &QAction::triggered, this,
          &ProgressListView::cancelTask);
  connect(m_open_dir_ac, &QAction::triggered, this, &ProgressListView::openDir);
  connect(m_clear_ac, &QAction::triggered, this,
          &ProgressListView::clearFinished);
}

ProgressListView::~ProgressListView() {}

void ProgressListView::onReceiverContextMenuRequested(const QPoint &pos) {
  QModelIndex index = indexAt(pos);
  if (index.isValid()) {
    QPoint glob_pos = mapToGlobal(pos);
    auto m = model();
    auto data = m->data(index, MyRole::IdRole);
    if (!data.isNull()) {
      m_selected_task = static_cast<qintptr>(data.toLongLong());
    }
    m_right_menu->exec(glob_pos);
  } else {
    m_selected_task = -1;
  }
}

void ProgressListView::cancelTask() {
  qDebug() << "Cancel " << m_selected_task;
  emit cancelSendTask(m_selected_task);
}

void ProgressListView::openDir() {
  QDesktopServices::openUrl(QUrl::fromLocalFile(Setting::ins().m_download_dir));
}