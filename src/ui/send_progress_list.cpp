#include "send_progress_list.h"

#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QProcess>

#include "model/send_progress_model.h"
#include "setting.h"

using TransferProgress::Column;

SendProgressListView::SendProgressListView(QWidget *parent)
    : QTableView(parent) {
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
  connect(this, &SendProgressListView::customContextMenuRequested, this,
          &SendProgressListView::onCustomRightMouseButtonPressed);

  m_right_menu = new QMenu(this);
  m_cancel_ac = new QAction("Cancel", this);
  m_open_dir_ac = new QAction("Open In Dir", this);
  m_clear_ac = new QAction("Clear", this);
  // m_right_menu->addAction(m_cancel_ac);
  m_right_menu->addAction(m_open_dir_ac);
  m_right_menu->addAction(m_clear_ac);
  // disable all buttons default, enable in case
  m_cancel_ac->setDisabled(true);
  m_open_dir_ac->setDisabled(true);
  m_clear_ac->setDisabled(true);
  connect(m_cancel_ac, &QAction::triggered, this,
          &SendProgressListView::onCancelButtonClicked);
  connect(m_open_dir_ac, &QAction::triggered, this,
          &SendProgressListView::onOpenDirButtonClicked);
  connect(m_clear_ac, &QAction::triggered, this,
          &SendProgressListView::onClearButtonClicked);
}

void SendProgressListView::onCustomRightMouseButtonPressed(const QPoint &pos) {
  m_selected_task.clear();
  auto m = model();
  QModelIndex index = indexAt(pos);
  if (index.isValid()) {
    auto task = m->data(index, MyRole::InfoRole);
    if (task.isValid()) {
      auto info = task.value<TransferInfo>();
      if (m_selected_task.count(info) == 0) {
        m_selected_task.append(info);
      }
    }
    // get mouse pos
    QPoint glob_pos = mapToGlobal(pos);

    auto task_state = m_selected_task.front().m_state;
    // show right mouse menu
    const auto &actions = m_right_menu->actions();
    if (task_state == TransferState::Canceled ||
        task_state == TransferState::Disconnected ||
        task_state == TransferState::Finish ||
        task_state == TransferState::Rejected) {
      // enable open and clear
      actions.at(0)->setEnabled(true);
      actions.at(1)->setEnabled(true);
    } else if (task_state == TransferState::Transfering ||
               task_state == TransferState::Pending) {
      // enable cancel and open
      actions.at(0)->setEnabled(true);
      // actions.at(1)->setEnabled(true);
    }

    // show right mouse menu
    m_right_menu->exec(glob_pos);
    for (auto &&i : actions) {
      i->setDisabled(true);
    }
  }
}

void SendProgressListView::onCancelButtonClicked() {
  emit cancelSendTask(m_selected_task);
  m_selected_task.clear();
}

void SendProgressListView::onOpenDirButtonClicked() {
  auto selected_file_path = m_selected_task.front().m_file_path;
#if defined(Q_OS_WIN)
  // use windows file explorer show file
  const QString explorer = "explorer";
  QStringList param;
  if (!QFileInfo(selected_file_path).isDir())
    param << QLatin1String("/select,");
  param << QDir::toNativeSeparators(selected_file_path);
  QProcess::startDetached(explorer, param);
#else
  auto file_path = QUrl::fromLocalFile(selected_file_path);
  auto dir = file_path.path();
  dir.truncate(file_path.path().size() - file_path.fileName().size());
  QDesktopServices::openUrl(dir);
#endif
}

void SendProgressListView::onClearButtonClicked(){
  emit clearFinished(m_selected_task);
  m_selected_task.clear();
}