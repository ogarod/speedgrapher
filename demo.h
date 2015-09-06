#ifndef DEMO_H_
#define DEMO_H_

#include "speed-grapher.h"

class SpeedGrapherDemo : public QWidget {
  Q_OBJECT

 public:
  SpeedGrapherDemo();
  virtual ~SpeedGrapherDemo();

 public slots:
  void Run();

 private:
  SpeedGrapher* speed_grapher_;
  QTimer* timer_;
  int num_points_;
  double num_oscillations_;
  int interval_;
  std::vector<double> ys_;
  int iter_;
};
#endif // DEMO_H_
