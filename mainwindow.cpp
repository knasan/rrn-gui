#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QDirIterator>
#include <QTableWidget>
#include <QtConcurrent>
#include <QThread>
#include <QStatusBar>


// TODO: ein Connect erstellen das ein bestimmtes message ausgeblendet wird, wenn in das Feld etwas eingetragen wird.
// Gibt es ein Trigger wenn man in das feld klickt, das der eintrag gelöscht wird?
// Dies könnte man mit den QTimer machen, so wurde es auch im C++ Installer mal gemacht. EVtl. dort mal reinschauen.
// Kontextmenü für die Tabellen. Move (also verschieben)! Hierfür benötigt man vermutlich einen Connect von SIGNAL und SLOT
// das die ausgewählte spalte entfernt wird oder eben wieder eingefügt wird.



QStringList collectDataFiles;
void getFiles(QString dir, QStringList *collectDataFiles);
void workerGetFiles(QString dir);

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

  // Statusbar as Wizard
  ui->statusbar->showMessage(tr("click button 'select directory'"), 0);

  ui->label_message_doit->setText("");
  ui->label_message_excludes->setText("");
  ui->label_message_search->setText("");
  ui->label_message_replace->setText("");

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
      ui->lineEdit_Search->setEnabled(true);
      ui->lineEdit_Replace->setEnabled(true);

      // Statusbar Message
      ui->statusbar->showMessage(tr("click button collect data. This can take several minutes"), 0);
    }
}

/*
 * pushButton SIGNAL:
 * pressed
 * released
 */

void MainWindow::on_pushButton_Collect_pressed()
{
  qDebug() << "pushButton Collect pressed";

  // Disable Collect Data;
  ui->pushButton_Collect->setDisabled(true);
}

void MainWindow::on_pushButton_Collect_released()
{
  qDebug() << "pushButton Collect released";

  // Clear the Table for New Results
  ui->tableResult->clearContents();
  collectDataFiles.clear();

  QString dir = ui->lineEdit_Destination->text();
  ui->tableResult->setRowCount(0);
  workerGetFiles(dir);

  // Enable Button CollectData
  ui->pushButton_Collect->setDisabled(false);
  ui->pushButton_DoIt->setDisabled(false);

  // Fill tableResult
  int row = 0, col = 0;
  for (int i = 0; i < collectDataFiles.size(); i++)
    {
      insertTableResult(row, col, collectDataFiles[i]);
      row++;
    }
  ui->tableExclude->setRowCount(row);
  ui->statusbar->showMessage(tr("You can move files that you want to exclude to the exclude table."), 0);

  QString message_search = tr("Enter a character or word in the search field that you want to search.");
  QString message_replace = tr("Enter a character or word in the replace field.");
  QString message_exclude = tr("You can move files that you want to exclude to the excludes table.");
  QString message_doit = tr("When you are satisfied, press the 'Do it' button.\n");

  ui->label_message_search->setText(message_search);
  ui->label_message_replace->setText(message_replace);
  ui->label_message_excludes->setText(message_exclude);
  ui->label_message_doit->setText(message_doit);

}

// is pressed and released on, code ignore this.
/*void MainWindow::on_pushButton_Collect_clicked()
{
  qDebug() << "pushButton Collect clicked";
}

void MainWindow::on_pushButton_Collect_toggled()
{
  qDebug() << "pushButton Collect toggled";
}*/

void MainWindow::insertTableResult(int row, int col, QString text)
{
  ui->tableResult->insertRow(row);
  ui->tableResult->setItem(row, col, new QTableWidgetItem(text));
}

/*
 *
 */
void workerGetFiles(QString dir)
{
  // collectDataFiles Threaded
   QFuture<void> t1 = QtConcurrent::run(getFiles, dir, &collectDataFiles);
   t1.waitForFinished();
}

void getFiles(QString dir, QStringList *collect)
{
  QDirIterator it(dir, QDirIterator::Subdirectories);
  while (it.hasNext()) {
      if(it.fileName() == "." || it.fileName() == ".." || it.fileName() == "")
        {
          it.next();
          continue;
        }
      collect->append(it.fileInfo().absoluteFilePath());
      it.next();
    }
}
