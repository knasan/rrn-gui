#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->tableResult->setDragEnabled(true);
  ui->tableExclude->setDragEnabled(true);
  ui->tableResult->setDragDropMode(QAbstractItemView::DragDrop);
  ui->tableExclude->dragEnabled();
  ui->tableResult->dragEnabled();
  ui->tableExclude->setDragDropMode(QAbstractItemView::DragDrop);

  // Gitter ausschalten
  ui->tableResult->setShowGrid(false);
  ui->tableExclude->setShowGrid(false);

  // Fake einträge
  QString entry[] = {
    "/home/users/jon",
    "/home/users/smk",
    "/home/users/joerg",
    "/home/users/michael",
  };

  // TODO: warum kann ich von Result nach Exclude und eintrag wird aus Result nicht gelöscht (was auch sinn machen kann),
  // anders herum wird jedoch der eintrag gelöscht. Testen, mal einen Manuellen Eintrag machen. Mal sehen ob es sich dann noch immer so verhält.
  // wenn ja muss ich herausfinden wie man das genau konfiguriert.
  // Manuell hinzugefügte daten werden nicht via drag and drop gelöscht. Wenn rrn die Tabelle dynamisch füllt, könnte dies anders aussehen.

  int column = 0;
  int row = 0;
  ui->tableResult->insertColumn(column);
  ui->tableExclude->insertColumn(column);
  ui->tableExclude->setColumnWidth(column, 300);
  ui->tableExclude->insertRow(row);
  ui->tableExclude->setItem(row, column, new QTableWidgetItem{"Testeintrag"});
  ui->tableResult->setToolTip("Drag And Drop Elements.");
  ui->tableExclude->setToolTip("Drag from Results Table enabled");
  for (int i = 0; i < 4; i++)
    {
      ui->tableResult->insertRow(row);
      ui->tableResult->setColumnWidth(column, 300);
      ui->tableResult->setItem(row, column, new QTableWidgetItem{entry[i]} );
      row++;
    }
}

MainWindow::~MainWindow()
{
  delete ui;
}

