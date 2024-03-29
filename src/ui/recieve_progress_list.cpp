#include "recieve_progress_list.h"

#include <QDesktopServices>
#include <QFileDialog>
// #include <QFileSystemModel>
#include <QHeaderView>
#include <QProcess>

#include "model/recieve_progress_model.h"
#include "setting.h"

using TransferProgress::Column;

RecieveProgressListView::RecieveProgressListView(QWidget *parent)
    : QTableView(parent) {
  setSelectionMode(QAbstractItemView::SingleSelection);
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
  connect(this, &RecieveProgressListView::customContextMenuRequested, this,
          &RecieveProgressListView::onCustomRightMouseButtonPressed);

  m_right_menu = new QMenu(this);
  m_accept_ac = new QAction("Accept", this);
  m_reject_ac = new QAction("Reject", this);
  m_cancel_ac = new QAction("Cancel", this);
  m_open_dir_ac = new QAction("Open In Dir", this);
  m_clear_ac = new QAction("Clear", this);
  m_right_menu->addAction(m_accept_ac);
  m_right_menu->addAction(m_reject_ac);
  // m_right_menu->addAction(m_cancel_ac);
  m_right_menu->addAction(m_open_dir_ac);
  m_right_menu->addAction(m_clear_ac);
  // disable all buttons default, enable in case
  m_accept_ac->setDisabled(true);
  m_reject_ac->setDisabled(true);
  m_cancel_ac->setDisabled(true);
  m_open_dir_ac->setDisabled(true);
  m_clear_ac->setDisabled(true);
  connect(m_accept_ac, &QAction::triggered, this,
          &RecieveProgressListView::onAcceptButtonClicked);
  connect(m_reject_ac, &QAction::triggered, this,
          &RecieveProgressListView::onRejectButtonClicked);
  connect(m_cancel_ac, &QAction::triggered, this,
          &RecieveProgressListView::onCancelButtonClicked);
  connect(m_open_dir_ac, &QAction::triggered, this,
          &RecieveProgressListView::onOpenDirButtonClicked);
  connect(m_clear_ac, &QAction::triggered, this,
          &RecieveProgressListView::onClearButtonClicked);
}

void RecieveProgressListView::onCustomRightMouseButtonPressed(
    const QPoint &pos) {
  m_selected_task.clear();
  QModelIndex index = indexAt(pos);
  if (index.isValid()) {
    auto task = model()->data(index, MyRole::InfoRole);
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
      actions.at(2)->setEnabled(true);
      actions.at(3)->setEnabled(true);
    } else if (task_state == TransferState::Transfering) {
      // enable cancel and open
      actions.at(2)->setEnabled(true);
      // actions.at(3)->setEnabled(true);
    } else if (task_state == TransferState::Pending) {
      // enable accept and reject
      actions.at(0)->setEnabled(true);
      actions.at(1)->setEnabled(true);
    }
    m_right_menu->exec(glob_pos);
    for (auto &&i : actions) {
      i->setDisabled(true);
    }
  }
}

void RecieveProgressListView::onAcceptButtonClicked() {
  auto selected_task = m_selected_task;
  m_selected_task.clear();
  emit acceptSendTask(selected_task);
}

void RecieveProgressListView::onRejectButtonClicked() {
  auto selected_task = m_selected_task;
  m_selected_task.clear();
  emit rejectSendTask(selected_task);
}

void RecieveProgressListView::onCancelButtonClicked() {
  auto selected_task = m_selected_task;
  m_selected_task.clear();
  emit cancelSendTask(m_selected_task);
}

void RecieveProgressListView::onOpenDirButtonClicked() {
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

void RecieveProgressListView::onClearButtonClicked() {
  emit clearFinished(m_selected_task);
  m_selected_task.clear();
}