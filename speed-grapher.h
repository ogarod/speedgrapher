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

#ifndef SPEED_GRAPHER_H_
#define SPEED_GRAPHER_H_

#define _USE_MATH_DEFINES 1;
#include <math.h>
#include <vector>

#include <QGraphicsView>
#include <QObject>
#include <QPainterPath>
#include <QSize>
#include <QString>
#include <QVariant>

// A widget for real-time visualization of speed and progress - the combination
// of a line graph and a progress bar.
class SpeedGrapher : public QGraphicsView {
 public:
  enum StyleAttribute {
    BACKGROUND_BRUSH_COLOR,
    BACKGROUND_BRUSH_ALPHA,
    GRID_PEN_COLOR,
    GRID_PEN_ALPHA,
    GRID_PEN_WIDTH,
    CURVE_PEN_COLOR,
    CURVE_PEN_ALPHA,
    CURVE_PEN_WIDTH,
    CURVE_BRUSH_COLOR,
    CURVE_BRUSH_ALPHA,
    CHART_BRUSH_COLOR,
    CHART_BRUSH_ALPHA,
    INDICATOR_DOT_PEN_COLOR,
    INDICATOR_DOT_PEN_ALPHA,
    INDICATOR_DOT_PEN_WIDTH,
    INDICATOR_DOT_BRUSH_COLOR,
    INDICATOR_DOT_BRUSH_ALPHA,
    INDICATOR_DOT_SIZE,
    INDICATOR_LINE_PEN_COLOR,
    INDICATOR_LINE_PEN_ALPHA,
    INDICATOR_LINE_PEN_WIDTH,
    INDICATOR_TEXT_FONT_NAME,
    INDICATOR_TEXT_FONT_SIZE,
    INDICATOR_TEXT_COLOR,
    INDICATOR_TEXT_ALPHA
  };

  SpeedGrapher(QWidget* parent,
               const QSize& size,
               int num_vertical_gridlines,
               int num_horizontal_gridlines);
  SpeedGrapher(QWidget* parent, const QSize& size);
  SpeedGrapher(QWidget* parent);

  // Adds a new data point to the speed grapher.
  void AddDataPoint(double y, double progress,
                    const QString& indicator_text);
  void AddDataPoint(double y, double progress);

  // Refreshes the plot. You only need to call this directly if you do not
  // want the indicators shown (horizontal indicator line and caption).
  void UpdatePlot(bool show_indicators);

  // Getters and setters.
  const std::vector<double>& GetData();
  void SetData(const std::vector<double>& ys);
  void SetProgress(const double progress);
  QVariant GetStyleAttribute(StyleAttribute style_attr);
  double GetDoubleStyleAttribute(StyleAttribute style_attr);
  QString GetStringStyleAttributeS(StyleAttribute style_attr);
  void SetStyleAttribute(StyleAttribute style_attr,
                         const QVariant& style_value);

 private:
  // Constructs a QPainterPath through all data points.
  QPainterPath* ComputeSeriesPath(double* last_x, double* last_y);

  void DrawGridlines();

  // Helper method for creating pens.
  QPen* MkPen(StyleAttribute color_attr,
                       StyleAttribute alpha_attr,
                       StyleAttribute width_attr);

  // Helper method for creating brushes.
  QBrush* MkBrush(StyleAttribute color_attr,
                  StyleAttribute alpha_attr);

  QSize size_;
  std::vector<double> ys_;
  double progress_;
  QMap<StyleAttribute, QVariant> style_dict_;
  QGraphicsScene scene_;
  double yprop_;
  int num_vertical_gridlines_;
  int num_horizontal_gridlines_;
  double indicator_text_margin_bottom_;
  double indicator_text_margin_right_;
  QString indicator_text_;
};

#endif // SPEED_GRAPHER_H_
