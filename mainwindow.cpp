#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QDirIterator>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  // Enable Drag and Drop in QTableWidget
  ui->tableResult->setDragEnabled(true);
  ui->tableResult->dragEnabled();
  ui->tableResult->setDragDropMode(QAbstractItemView::DragDrop);

  ui->tableExclude->setDragEnabled(true);
  ui->tableExclude->dragEnabled();
  ui->tableExclude->setDragDropMode(QAbstractItemView::DragDrop);

  // Grid disabled
  ui->tableResult->setShowGrid(false);
  ui->tableExclude->setShowGrid(false);

  // Enabled sorted
  ui->tableResult->setSortingEnabled(true);
  ui->tableExclude->setSortingEnabled(true);

  // 100% Stretch Column
  ui->tableResult->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  ui->tableExclude->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableExclude->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableExclude->verticalHeader()->setVisible(false);

  // Disable Buttons
  ui->pushButton_DoIt->setDisabled(true);
  ui->pushButton_Collect->setDisabled(true);

  // Disable LineEdit
  ui->lineEdit_Search->setDisabled(true);
  ui->lineEdit_Replace->setDisabled(true);

}

MainWindow::~MainWindow()
{
  delete ui;
}

// Slot PushButton Destination.
void MainWindow::on_pushButton_Destination_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(
        this, tr("Open Directory"),
        QDir().currentPath(),
        QFileDialog::ShowDirsOnly
        | QFileDialog::DontResolveSymlinks
        | QFileDialog::ReadOnly);

  if (dir != "")
    {
      // Enable Collect button
      ui->pushButton_Collect->setEnabled(true);
      // set directory in the LineEdit Destination
      ui->lineEdit_Destination->setText(dir);

      // Enable Search and Replace LineEdit
      ui->lineEdit_Search->setDisabled(false);
      ui->lineEdit_Replace->setDisabled(false);
    }
}

void MainWindow::listDirectory(QString dir)
{
  QDirIterator it(dir, QDirIterator::Subdirectories);
  int row = 0, col = 0;
  while (it.hasNext()) {
      if(it.fileName() == "." || it.fileName() == ".." || it.fileName() == "")
        {
          it.next();
          continue;
        }
      ui->tableResult->insertRow(row);
      ui->tableResult->setItem(row,col, new QTableWidgetItem(it.path()+"/"+it.fileName()));
      row++;
      it.next();
    }
  ui->tableExclude->setRowCount(row);
}

void MainWindow::on_pushButton_Collect_clicked()
{
  // Clear the Table for New Results
  ui->tableResult->clearContents();
  ui->tableResult->setRowCount(0);

  // listDirectory fill the Table (TODO: refactor this name)
  listDirectory(ui->lineEdit_Destination->text());
}
