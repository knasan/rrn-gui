#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_pushButton_Destination_clicked();
  void on_pushButton_Collect_released();
  void on_pushButton_Collect_pressed();
  void on_tableResult_itemDoubleClicked(QTableWidgetItem *item);

  void on_tableExclude_itemDoubleClicked(QTableWidgetItem *item);

 signals:

 private:
  Ui::MainWindow *ui;
  void insertTableResult(int row, int col, QString text);
  // Check ist item->text() a directory? for each contain the text
  // Delete fromTbl and copy to toTbl
  void directoryTableCheck(QTableWidgetItem *item, QTableWidget *fromTbl,
                           QTableWidget *toTbl);
};
#endif  // MAINWINDOW_H
