#include "mainwindow.h"

#include <QDebug>
#include <QDirIterator>
#include <QFileDialog>
#include <QLineEdit>
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

  // TODO: DragDrop
  /*
    ui->tableResult->setDragEnabled(false);
    ui->tableExclude->setDragEnabled(false);

    ui->tableResult->setDragDropMode(QAbstractItemView::NoDragDrop);  //
    DragOnly ui->tableResult->setDragDropOverwriteMode(false);

    ui->tableExclude->setDragDropMode(QAbstractItemView::NoDragDrop);
    ui->tableExclude->setDragDropOverwriteMode(false);

    // Grid disabled
    // ui->tableResult->setShowGrid(false);
    // ui->tableExclude->setShowGrid(false);
  */
  // Enabled sorted
  ui->tableResult->setSortingEnabled(true);
  ui->tableExclude->setSortingEnabled(true);

  // 100% Stretch Column
  ui->tableResult->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableResult->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  ui->tableExclude->verticalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->tableExclude->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->tableExclude->verticalHeader()->setVisible(false);

  // Enable Sorting
  ui->tableResult->setSortingEnabled(true);
  ui->tableExclude->setSortingEnabled(true);

  // Sort by Name
  ui->tableExclude->sortItems(0, Qt::SortOrder::AscendingOrder);
  ui->tableExclude->sortItems(0, Qt::SortOrder::AscendingOrder);

  // Disable Buttons
  ui->pushButton_DoIt->setDisabled(true);
  ui->pushButton_Collect->setDisabled(true);

  // Disable LineEdit
  ui->lineEdit_Search->setDisabled(true);
  ui->lineEdit_Replace->setDisabled(true);

  // Statusbar as Wizard
  ui->statusbar->showMessage(tr("click button 'select directory'"), 0);

  ui->label_message_doit->setText("");
  ui->label_message_excludes->setText("");
  ui->label_message_search->setText("");
  ui->label_message_replace->setText("");
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
    ui->statusbar->showMessage(
        tr("click button collect data. This can take several minutes"), 0);
  }
}

/*
 * pushButton SIGNAL:
 * pressed
 * released
 */

// Disable Collect Data Button;
void MainWindow::on_pushButton_Collect_pressed() {
  ui->pushButton_Collect->setDisabled(true);
}

void MainWindow::on_pushButton_Collect_released() {
  // Clear the Table for New Results
  ui->tableResult->clearContents();
  collectDataFiles.clear();

  QString dir = ui->lineEdit_Destination->text();
  ui->tableResult->setRowCount(0);
  ui->tableExclude->setRowCount(0);
  workerGetFiles(dir);

  // Enable Button CollectData
  ui->pushButton_Collect->setDisabled(false);
  ui->pushButton_DoIt->setDisabled(false);

  // Fill tableResult
  int row = 0, col = 0;
  for (int i = 0; i < collectDataFiles.size(); i++) {
    insertTableResult(row, col, collectDataFiles[i]);
    row++;
  }

  ui->statusbar->showMessage(tr("You can move files that you want to exclude "
                                "to the exclude table with doubleclick."),
                             0);

  QString message_search =
      tr("Enter a character or word into the search box that you want to "
         "search.");
  QString message_replace =
      tr("Enter a character or word into the replace box.");
  QString message_exclude =
      tr("You can move files that you want to exclude to the excludes table "
         "with doubleclick.");
  QString message_doit = tr("If you are satisfied, press the 'Do it' button.");

  ui->label_message_search->setText(message_search);
  ui->label_message_replace->setText(message_replace);
  ui->label_message_excludes->setText(message_exclude);
  ui->label_message_doit->setText(message_doit);
}

void MainWindow::insertTableResult(int row, int col, QString text) {
  ui->tableResult->insertRow(row);
  ui->tableResult->setItem(row, col, new QTableWidgetItem(text));
}

void MainWindow::on_tableResult_itemDoubleClicked(QTableWidgetItem *item) {
  switchTable(item, ui->tableResult, ui->tableExclude);
}

void MainWindow::switchTable(QTableWidgetItem *item, QTableWidget *fromTbl,
                             QTableWidget *toTbl) {
  QString itemText = item->text();
  int rowCountToTbl = toTbl->rowCount();
  toTbl->insertRow(rowCountToTbl);
  toTbl->setItem(rowCountToTbl, 0, new QTableWidgetItem(item->text()));
  fromTbl->removeRow(item->row());
}

void MainWindow::on_tableExclude_itemDoubleClicked(QTableWidgetItem *item) {
  switchTable(item, ui->tableExclude, ui->tableResult);
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
