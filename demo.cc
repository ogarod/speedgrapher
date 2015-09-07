#include "demo.h"
#include <QTimer>
#include <QVBoxLayout>
#include <QDebug>

SpeedGrapherDemo::SpeedGrapherDemo()
    : num_points_(400),
      num_oscillations_(4.8),
      interval_ms_(100),
      iter_(0) {
  timer_ = new QTimer(this);
  speed_grapher_ = new SpeedGrapher(this);
  // Compute data points for the function abs(sin(Kx) where K is a constant.
  double K = 2 * M_PI * num_oscillations_ / (num_points_ - 1);
  for (int i = 0; i < num_points_; ++i) {
    ys_.push_back(fabs(sin(K * i)) + qrand() / (double) RAND_MAX);
  }
  connect(timer_, SIGNAL(timeout()), this, SLOT(Step()));
  show();
  timer_->start(interval_ms_);
}

SpeedGrapherDemo::~SpeedGrapherDemo() {}

void SpeedGrapherDemo::Step() {
 if (iter_ < num_points_) {
   double progress = (iter_ + 1.0) / num_points_;
   speed_grapher_->AddDataPoint(ys_[iter_], progress);
   iter_ += 1;
 } else {
   speed_grapher_->UpdatePlot(false);
 }
}
