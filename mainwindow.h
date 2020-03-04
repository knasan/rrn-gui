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
  void on_pushButton_DoIt_clicked();
  void on_lineEdit_Search_editingFinished();
  void on_lineEdit_Replace_editingFinished();

 signals:

 private:
  Ui::MainWindow *ui;
  void insertTableFilesToRename(int row, int col, QString text);
  void message(QString msg);
};
#endif  // MAINWINDOW_H
