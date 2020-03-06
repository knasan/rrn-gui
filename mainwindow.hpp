#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QMainWindow>
#include <QMenu>
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
  void customMenuRequested(QPoint pos);
  void on_actionAdd_exclude_triggered();
  void on_lineEdit_Replace_editingFinished();
  void on_lineEdit_Search_editingFinished();
  void on_pushButton_Collect_pressed();
  void on_pushButton_Collect_released();
  void on_pushButton_Destination_clicked();
  void on_pushButton_DoIt_clicked();
  void on_tableFilesToRename_itemDoubleClicked(QTableWidgetItem *item);
  void switchColum(bool trigger);

 signals:

 private:
  Ui::MainWindow *ui;
  void addContextMenu();
  void defaultSettings();
  void insertTableFilesToRename(int row, int col, QString text);
  void message(QString msg);
  void tableSetting();
};
#endif  // MAINWINDOW_H