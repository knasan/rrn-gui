#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QLineEdit>

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

  // Disable Buttons
  ui->pushButton_DoIt->setDisabled(true);
  ui->pushButton_Collect->setDisabled(true);

  // Disable LineEdit
  ui->lineEdit_Search->setDisabled(true);
  ui->lineEdit_Replace->setDisabled(true);

  // Hide Progressbar
  //ui->progressBar->setHidden(true);
}

MainWindow::~MainWindow()
{
  delete ui;
}

// Slot Push Button Destination.
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
      ui->pushButton_Collect->setEnabled(true);
      ui->lineEdit_Destination->setText(dir);
      ui->lineEdit_Search->setDisabled(false);
      ui->lineEdit_Replace->setDisabled(false);

      // statt einen Button, könnte man auch sofort mit dem einlesen beginnen.
      // nachteil wäre hier, das der Benutzer nicht mehr die Wahl hat dies zu korrigieren.
      // Bei diese Methode muss der Benutzer aktiv werden um die Daten einzusammeln.
      // Was mehr Kontrolle verspricht.
    }
}

