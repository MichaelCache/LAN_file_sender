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
    auto id_data = m->data(index, MyRole::IdRole);
    if (!id_data.isNull()) {
      m_selected_task = id_data.toUuid();
    }

    auto path_data = m->data(index, MyRole::PathRole);
    if (!path_data.isNull()) {
      m_selected_file_path = path_data.toString();
    }
    // show right mouse menu
    m_right_menu->exec(glob_pos);
  } else {
    m_selected_task = QUuid();
  }
}

void ProgressListView::cancelTask() {
  qDebug() << "Cancel " << m_selected_task;
  emit cancelSendTask(m_selected_task);
}

void ProgressListView::openDir() {
#if defined(Q_OS_WIN)
  // use windows file explorer show file
  const QString explorer = "explorer";
  QStringList param;
  if (!QFileInfo(m_selected_file_path).isDir())
    param << QLatin1String("/select,");
  param << QDir::toNativeSeparators(m_selected_file_path);
  QProcess::startDetached(explorer, param);
#else
  auto file_path = QUrl::fromLocalFile(m_selected_file_path);
  auto dir = file_path.path();
  dir.truncate(file_path.path().size()- file_path.fileName().size());
  QDesktopServices::openUrl(dir);
#endif
}