#include "demo.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  SpeedGrapherDemo d;
  app.exec();
}
