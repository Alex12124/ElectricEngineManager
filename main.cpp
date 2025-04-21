#include <QApplication>
#include <QMetaType>

#include "model/engine/engine.h"
#include "pages/mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  qRegisterMetaType<model::Engine>("model::Engine");
  pages::Main w;
  w.show();
  return a.exec();
}
