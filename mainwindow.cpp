#include "mainwindow.h"

#include <QDebug>
#include <QDirIterator>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QThread>
#include <QtConcurrent>

#include "ui_mainwindow.h"

// TODO: ein Connect erstellen das ein bestimmtes message ausgeblendet wird,
// wenn in das Feld etwas eingetragen oder aktiviert ist/wird.

QStringList collectDataFiles;
void getFiles(QString dir, QStringList *collectDataFiles);
void workerGetFiles(QString dir);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // DragDrop
  ui->tableFilesToRename->setDragEnabled(true);

  // Context Menu
  // ui->tableFilesToRename->setContextMenuPolicy(Qt::CustomContextMenu);
  /*QObject::connect(ui->tableFilesToRename,
                   SIGNAL(customContextMenuRequested(QPoint)),
                   SLOT(customMenuRequested(QPoint)));*/

  // ui->tableFilesToRename->setDragDropMode(QAbstractItemView::DragDrop);
  // ui->tableFilesToRename->setAttribute(Qt::WidgetAttribute::WA_AcceptDrops);
  // ui->tableFilesToRename->setDefaultDropAction(Qt::DropAction::MoveAction);

  // Disbable Row/Column edit
  ui->tableFilesToRename->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableResult->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // Grid disabled
  // ui->tableFilesToRename->setShowGrid(false);

  // 100% Stretch Column
  ui->tableFilesToRename->verticalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->tableFilesToRename->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->tableResult->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  // Enable Sorting
  ui->tableFilesToRename->setSortingEnabled(false);
  ui->tableResult->setSortingEnabled(false);

  // Sort by Name
  ui->tableFilesToRename->sortItems(0, Qt::SortOrder::AscendingOrder);
  ui->tableResult->sortItems(1, Qt::SortOrder::AscendingOrder);

  // Disable Buttons
  ui->pushButton_DoIt->setDisabled(true);
  ui->pushButton_Collect->setDisabled(true);

  // Disable LineEdit
  ui->lineEdit_Search->setDisabled(true);
  ui->lineEdit_Replace->setDisabled(true);

  // Statusbar as Wizard
  QString msg = tr("click button 'select directory'");
  ui->statusbar->showMessage(msg, 0);
  ui->labelMessage->setText(msg);

  // tableRename Hide
  ui->tableResult->hide();
}

MainWindow::~MainWindow() { delete ui; }

// Slot PushButton Destination.
void MainWindow::on_pushButton_Destination_clicked() {
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Open Directory"), QDir().currentPath(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks |
          QFileDialog::ReadOnly);

  if (dir != "") {
    // Enable Collect button
    ui->pushButton_Collect->setEnabled(true);
    // set directory in the LineEdit Destination
    ui->lineEdit_Destination->setText(dir);

    // Enable Search and Replace LineEdit
    ui->lineEdit_Search->setEnabled(true);
    ui->lineEdit_Replace->setEnabled(true);

    // Statusbar Message
    QString msg =
        tr("click button collect data. This can take several minutes");
    ui->statusbar->showMessage(msg, 0);
    ui->labelMessage->setText(msg);
  }
}

/*
 * pushButton SIGNAL:
 * pressed
 * released
 */

// Disable Collect Data Button;
void MainWindow::on_pushButton_Collect_pressed() {
  ui->tableResult->hide();
  ui->tableFilesToRename->show();
  ui->pushButton_Collect->setDisabled(true);
}

void MainWindow::on_pushButton_Collect_released() {
  // Clear the Table for New Results
  ui->tableFilesToRename->clearContents();
  collectDataFiles.clear();

  QString dir = ui->lineEdit_Destination->text();
  ui->tableFilesToRename->setRowCount(0);
  workerGetFiles(dir);

  // Enable Button CollectData
  ui->pushButton_Collect->setDisabled(false);
  ui->pushButton_DoIt->setDisabled(false);

  // Fill tableFilesToRename
  int row = 0, col = 0;
  for (int i = 0; i < collectDataFiles.size(); i++) {
    insertTableFilesToRename(row, col, collectDataFiles[i]);
    row++;
  }

  ui->statusbar->showMessage(tr("You can move files that you want to exclude "
                                "to the exclude table with doubleclick."),
                             0);
  QString message_search =
      tr("Enter a character or word into the search box that you want to "
         "search.<br>If you want to use a whitespace as a search or replace "
         "element, leave one of the input fields empty!");
  ui->labelMessage->setText(message_search);
}

void MainWindow::insertTableFilesToRename(int row, int col, QString text) {
  ui->tableFilesToRename->insertRow(row);
  ui->tableFilesToRename->setItem(row, col, new QTableWidgetItem(text));
}

void MainWindow::message(QString msg) {
  QMessageBox::warning(this, tr("Error"), msg, QMessageBox::Ok);
}

// Button DoIt
void MainWindow::on_pushButton_DoIt_clicked() {
  QString search = ui->lineEdit_Search->text().simplified();
  QString replace = ui->lineEdit_Replace->text().simplified();

  QString msg;

  if (search.size() == 0 && replace.size() == 0) {
    msg = tr("search box or replacement box must not be empty.");
    message(msg);
    return;
  }

  if (search == replace) {
    msg = tr("search and replace box must not be the same.");
    message(msg);
    return;
  }

  if (search.size() == 0) {
    search = " ";
  }

  if (replace.size() == 0) {
    replace = " ";
  }

  QString newFilename;

  int rows = ui->tableFilesToRename->rowCount();

  ui->tableFilesToRename->hide();
  ui->tableResult->show();
  ui->tableResult->setRowCount(rows);

  for (int row = 0; row < rows; ++row) {
    QString origFileName = ui->tableFilesToRename->takeItem(row, 0)->text();
    if (origFileName.contains(search, Qt::CaseInsensitive)) {
      newFilename = origFileName;
      newFilename.replace(search, replace);
      ui->tableResult->setItem(row, 0, new QTableWidgetItem(origFileName));
      ui->tableResult->setItem(row, 1, new QTableWidgetItem(newFilename));
      // Function for Rename with Threads
      // Progressbar +1
    } else {
      ui->tableResult->setItem(row, 0, new QTableWidgetItem(origFileName));
    }
  }
  // Wait for Thread

  // Set RealRowCount by Result Table and DoIt button disabled
  ui->pushButton_DoIt->setDisabled(true);
  ui->statusbar->clearMessage();
}

void workerGetFiles(QString dir) {
  // collectDataFiles Threaded
  QFuture<void> t1 = QtConcurrent::run(getFiles, dir, &collectDataFiles);
  t1.waitForFinished();
}

void getFiles(QString dir, QStringList *collect) {
  QDirIterator it(dir, QDirIterator::Subdirectories);
  while (it.hasNext()) {
    if (it.fileName() == "." || it.fileName() == ".." || it.fileName() == "") {
      it.next();
      continue;
    }
    collect->append(it.fileInfo().absoluteFilePath());
    it.next();
  }
}

void MainWindow::on_lineEdit_Search_editingFinished() {
  QString message_replace =
      tr("Enter a character or word into the replace box.");
  ui->labelMessage->setText(message_replace);
}

void MainWindow::on_lineEdit_Replace_editingFinished() {
  QString message_exclude = tr(
      "You can move files that you want to exclude to the excludes table "
      "with doubleclick.<br>If you are satisfied, press the 'Do it' button.");
  ui->labelMessage->setText(message_exclude);
}

/*void MainWindow::customMenuRequested(QPoint pos) {
  // QModelIndex index = ui->tableFilesToRename->indexAt(pos);
  QMenu *menu = new QMenu(this);
  menu->addAction(new QAction(tr("add to exclude"), this));
  // menu->addAction(new QAction("Action 2", this));
  menu->popup(ui->tableFilesToRename->viewport()->mapToGlobal(pos));
}*/

void MainWindow::on_tableFilesToRename_itemDoubleClicked(
    QTableWidgetItem *item) {
  int column = item->column();
  int row = item->row();
  QString text = item->text();
  if (text == "") {
    return;
  }
  ui->tableFilesToRename->setItem(row, column, new QTableWidgetItem(""));
  // Column 0 is FiletoRename, 1 is Exclude
  if (column == 0) {
    // FilesToRename -> exclude
    column++;
  } else if (column == 1) {
    // Exclude <- FilesToRename
    column--;
  } else {
    qDebug() << "Unknown Column";
  }
  ui->tableFilesToRename->setItem(row, column, new QTableWidgetItem(text));
}
