/**
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

#ifndef DEMO_H_
#define DEMO_H_

#include "speed-grapher.h"

class SpeedGrapherDemo : public QWidget {
  Q_OBJECT

 public:
  SpeedGrapherDemo();
  virtual ~SpeedGrapherDemo();

 public slots:
  void Step();

 private:
  int num_points_; // Total number of data points the demo will generate.
  double num_oscillations_; // Number of oscillations for the demo function.
  int interval_ms_; // Interval between speedgrapher updates.
  std::vector<double> ys_; // Data points.
  int iter_; // A counter for iterations.
  QTimer* timer_;
  SpeedGrapher* speed_grapher_;
};
#endif // DEMO_H_
