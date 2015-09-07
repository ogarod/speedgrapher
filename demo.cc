/*
  The MIT License (MIT)

  Copyright (c) 2015 Denver Ogaro

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

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

  // Compute data points for the function abs(sin(Kx)) + r(x) where K is a
  // constant and r is a random function with the range [0, 1].
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
