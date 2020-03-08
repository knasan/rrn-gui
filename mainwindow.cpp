#include "mainwindow.hpp"

#include <QAction>
#include <QDebug>
#include <QDirIterator>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtConcurrent>

#include <filesystemutils.hpp>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  tableSetting();
  defaultSettings();
  addContextMenu();
}

// deconstructor
MainWindow::~MainWindow() { delete ui; }

// Private SLOTS
void MainWindow::customMenuRequested(QPoint pos) {
  qDebug() << "CustomMenuRequested, Pos: " << pos;

  // TODO: welche position ist RenameFile und Exclude?

  QMenu *menu = new QMenu(this);
  menu->addAction(tr("add exclude"), this,
                  SLOT(on_actionAdd_exclude_triggered()));
  // menu->addAction(new QAction("Action 2", this));
  menu->popup(ui->tableFilesToRename->viewport()->mapToGlobal(pos));
}

void MainWindow::on_actionAdd_exclude_triggered() {
  qDebug() << "Exclude trigger";
}

void MainWindow::on_lineEdit_Replace_editingFinished() {
  QString message_exclude = tr(
      "You can move files that you want to exclude to the excludes table "
      "with doubleclick.<br>If you are satisfied, press the 'Do it' button.");
  ui->labelMessage->setText(message_exclude);
}

void MainWindow::on_lineEdit_Search_editingFinished() {
  QString message_replace =
      tr("Enter a character or word into the replace box.");
  ui->labelMessage->setText(message_replace);
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
  QString dir = ui->lineEdit_Destination->text();
  ui->tableFilesToRename->setRowCount(0);

  // TODO: Start getFiles with QFuture
  QString dirpath = ui->lineEdit_Destination->text();
  filesystemUtils fsu;
  QStringList collectDataFiles = fsu.getFiles(dirpath);

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

  int rows = ui->tableFilesToRename->rowCount();

  ui->tableFilesToRename->hide();
  ui->tableResult->show();
  ui->tableResult->setRowCount(rows);

  filesystemUtils fsu;

  ui->tableFilesToRename->sortItems(Qt::SortOrder::DescendingOrder);
  for (int row = rows - 1; row >= 0; --row) {
    QString origFileName = ui->tableFilesToRename->takeItem(row, 0)->text();
    QString newFilename = fsu.renameFile(origFileName, search, replace);
    ui->tableResult->setItem(row, 0, new QTableWidgetItem(origFileName));
    if (newFilename != "") {
      ui->tableResult->setItem(row, 1, new QTableWidgetItem(newFilename));
    } else {
      ui->tableResult->setItem(row, 1, new QTableWidgetItem(origFileName));
    }
    // qDebug() << "o: " << origFileName << " To: " << newFilename;
  }
  ui->tableFilesToRename->sortItems(Qt::SortOrder::AscendingOrder);

  // Set RealRowCount by Result Table and DoIt button disabled
  ui->pushButton_DoIt->setDisabled(true);
  ui->statusbar->clearMessage();
}

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

void MainWindow::switchColum(bool trigger) {
  qDebug() << "Rename";
  qDebug() << "Trigger: " << trigger;
}
// Private Slot END

// Private Functions
void MainWindow::addContextMenu() {
  // Context Menu
  ui->tableFilesToRename->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(ui->tableFilesToRename,
                   SIGNAL(customContextMenuRequested(QPoint)),
                   SLOT(customMenuRequested(QPoint)));
}

void MainWindow::defaultSettings() {
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
}

void MainWindow::insertTableFilesToRename(int row, int col, QString text) {
  ui->tableFilesToRename->insertRow(row);
  ui->tableFilesToRename->setItem(row, col, new QTableWidgetItem(text));
}

void MainWindow::message(QString msg) {
  QMessageBox::warning(this, tr("Error"), msg, QMessageBox::Ok);
}

void MainWindow::tableSetting() {
  // DragDrop
  ui->tableFilesToRename->setDragEnabled(true);
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

  // tableRename Hide
  ui->tableResult->hide();
}
