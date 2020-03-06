#include <QApplication>
#include <QDebug>
#include <QTranslator>

#include "mainwindow.hpp"

int main(int argc, char *argv[]) {
  QTranslator translator;
  translator.load("rrn-gui_de_DE");

  QApplication a(argc, argv);
  a.installTranslator(&translator);
  MainWindow w;
  w.show();
  return a.exec();
}
