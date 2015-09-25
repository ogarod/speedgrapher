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

#include "speed-grapher.h"

#include <QDebug>
#include <QGraphicsTextItem>
#include <QScopedPointer>

using std::vector;

SpeedGrapher::SpeedGrapher(QWidget* parent, const QSize& size,
                           int num_vertical_gridlines,
                           int num_horizontal_gridlines)
    : QGraphicsView(parent),
      size_(size),
      num_vertical_gridlines_(num_vertical_gridlines),
      num_horizontal_gridlines_(num_horizontal_gridlines),
      yprop_(0.83),
      progress_(0.0),
      indicator_text_margin_bottom_(3.0),
      indicator_text_margin_right_(10.0) {
  style_dict_ = {
      {BACKGROUND_BRUSH_COLOR, "#fff"},
      {BACKGROUND_BRUSH_ALPHA, 1.0},
      {GRID_PEN_COLOR, "#ccc"},
      {GRID_PEN_ALPHA, 0.15},
      {GRID_PEN_WIDTH, 0.0},
      {CURVE_PEN_COLOR, "#060"},
      {CURVE_PEN_ALPHA, 1.0},
      {CURVE_PEN_WIDTH, 0.0},
      {CURVE_BRUSH_COLOR, "#0f0"},
      {CURVE_BRUSH_ALPHA, 0.5},
      {CHART_BRUSH_COLOR, "#0f0"},
      {CHART_BRUSH_ALPHA, 0.33},
      {INDICATOR_DOT_PEN_COLOR, "#ccc"},
      {INDICATOR_DOT_PEN_ALPHA, 1.0},
      {INDICATOR_DOT_PEN_WIDTH, 0.0},
      {INDICATOR_DOT_BRUSH_COLOR, "#0f0"},
      {INDICATOR_DOT_BRUSH_ALPHA, 0.5},
      {INDICATOR_DOT_SIZE, 8.0},
      {INDICATOR_LINE_PEN_COLOR, "#ccc"},
      {INDICATOR_LINE_PEN_ALPHA, 1.0},
      {INDICATOR_LINE_PEN_WIDTH, 0.0},
      {INDICATOR_TEXT_FONT_NAME, "Verdana"},
      {INDICATOR_TEXT_FONT_SIZE, 10.0},
      {INDICATOR_TEXT_COLOR, "#ccc"},
      {INDICATOR_TEXT_ALPHA, 1.0},
  };
  ys_.push_back(0); // First data point to plot.

  setScene(&scene_);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  scene_.setSceneRect(0, 0, size_.width(), size_.height());
  setFrameShape(QFrame::NoFrame);
  setFixedSize(size_);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  UpdatePlot(false);
  show();
}

SpeedGrapher::SpeedGrapher(QWidget* parent, const QSize& size)
  : SpeedGrapher(parent, size, 15, 10) {}

SpeedGrapher::SpeedGrapher(QWidget* parent)
  : SpeedGrapher(parent, QSize(1000, 200)) {}

void SpeedGrapher::UpdatePlot(bool show_indicators) {
  scene_.clear();

  // Draw gridlines.
  QScopedPointer<QBrush> background_brush(MkBrush(BACKGROUND_BRUSH_COLOR,
                                                  BACKGROUND_BRUSH_ALPHA));
  scene_.setBackgroundBrush(*background_brush);
  DrawGridlines();

  // Draw curve
  double last_x = 0;
  double last_y = 0;
  QScopedPointer<QPainterPath> path(ComputeSeriesPath(&last_x, &last_y));
  QScopedPointer<QPen> curve_pen(MkPen(CURVE_PEN_COLOR,
                                       CURVE_PEN_ALPHA,
                                       CURVE_PEN_WIDTH));
  scene_.addPath(*path, *curve_pen);

  // Shade area under the curve.
  QPainterPath area(*path);
  area.lineTo(last_x, size_.height());
  area.closeSubpath();
  scene_.addPath(area, Qt::NoPen, *MkBrush(CURVE_BRUSH_COLOR,
                                           CURVE_BRUSH_ALPHA));

  // Shade area behind the progress level.
  scene_.addRect(0, 0, progress_ * size_.width(), size_.height(),
                 Qt::NoPen, *MkBrush(CHART_BRUSH_COLOR,
                                     CHART_BRUSH_ALPHA));

  // If we are not supposed to show the indicators, return here.
  if (!show_indicators) {
    return;
  }

  // Draw indicator dot.
  double dot_size = style_dict_[INDICATOR_DOT_SIZE].toDouble();
  double dot_x = last_x - dot_size / 2;
  double dot_y = last_y - dot_size / 2;
  QScopedPointer<QPen> indicator_dot_pen(MkPen(INDICATOR_DOT_PEN_COLOR,
                                               INDICATOR_DOT_PEN_ALPHA,
                                               INDICATOR_DOT_PEN_WIDTH));
  QScopedPointer<QBrush> indicator_dot_brush(
        MkBrush(INDICATOR_DOT_BRUSH_COLOR, INDICATOR_DOT_BRUSH_ALPHA));
  scene_.addEllipse(dot_x, dot_y, dot_size, dot_size,
                    *indicator_dot_pen,
                    *indicator_dot_brush);

  // Draw horizontal indicator line.
  QScopedPointer<QPen> indicator_line_pen(MkPen(INDICATOR_LINE_PEN_COLOR,
                                                INDICATOR_LINE_PEN_ALPHA,
                                                INDICATOR_LINE_PEN_WIDTH));
  scene_.addLine(0, last_y+0.5, size_.width(), last_y+0.5,
                 *indicator_line_pen);

  // Draw a caption for the horizontal line.
  QString text = indicator_text_;
  if (text.isEmpty()) {
    text = QString::number(ys_.back());
  }
  QGraphicsTextItem* text_item =
      scene_.addText(text,
                     QFont(style_dict_[INDICATOR_TEXT_FONT_NAME].toString(),
                           style_dict_[INDICATOR_TEXT_FONT_SIZE].toDouble()));
  QColor text_color = QColor();
  text_color.setNamedColor(style_dict_[INDICATOR_TEXT_COLOR].toString());
  text_color.setAlphaF(style_dict_[INDICATOR_TEXT_ALPHA].toDouble());
  text_item->setDefaultTextColor(text_color);
  double text_x = (size_.width() - indicator_text_margin_right_ -
                   text_item->boundingRect().width());
  double text_y = (last_y - indicator_text_margin_bottom_ -
                   text_item->boundingRect().height());
  text_item->setPos(text_x, text_y < 0 ? 0 : text_y);
}

void SpeedGrapher::DrawGridlines() {
  QScopedPointer<QPen> grid_pen(MkPen(GRID_PEN_COLOR,
                                      GRID_PEN_ALPHA,
                                      GRID_PEN_WIDTH));

  int horizontal_interval = (int) (size_.width() / num_vertical_gridlines_);
  int vertical_interval = (int) (size_.height() / num_horizontal_gridlines_);

  // Draw vertical gridlines.
  for (int x = 0; x < size_.width(); x += horizontal_interval) {
    scene_.addLine(x + 0.5, 0, x + 0.5, size_.height(), *grid_pen);
  }

  // Draw horizontal gridlines.
  for (int y = 0; y < size_.height(); y += vertical_interval) {
    scene_.addLine(0, y + 0.5, size_.width(), y + 0.5, *grid_pen);
  }
}

QPainterPath* SpeedGrapher::ComputeSeriesPath(double* last_x, double* last_y) {
  // Calculate the x and y coordinates of the data points in the
  // GraphicsScene's coordinate system.
  vector<double> prepared_ys(ys_.begin(), ys_.end());
  vector<double> prepared_xs(ys_.begin(), ys_.end());
  double max_y = *std::max_element(prepared_ys.begin(), prepared_ys.end());
  if (max_y > 0) {
    for (int i = 0; i < prepared_ys.size(); ++i) {
      double y = prepared_ys[i];
      prepared_ys[i] = (1 - y / max_y * yprop_) * size_.height();
      prepared_xs[i] = i * progress_ * size_.width() / (prepared_xs.size() - 1);
    }
  }

  // Prepare a path through the data points.
  QPainterPath* path = new QPainterPath(QPointF(prepared_xs[0],
                                                prepared_ys[0]));
  for (int i = 0; i < prepared_ys.size(); ++i) {
    path->lineTo(prepared_xs[i], prepared_ys[i]);
  }
  *last_x = prepared_xs.back();
  *last_y = prepared_ys.back();
  return path;
}

QBrush* SpeedGrapher::MkBrush(StyleAttribute color_attr,
                              StyleAttribute alpha_attr) {
  QColor color;
  QString named_color = style_dict_[color_attr].toString();
  color.setNamedColor(named_color);
  color.setAlphaF(style_dict_[alpha_attr].toDouble());
  return new QBrush(color);
}

QPen* SpeedGrapher::MkPen(StyleAttribute color_attr,
                          StyleAttribute alpha_attr,
                          StyleAttribute width_attr) {
  QColor color;
  color.setNamedColor(style_dict_[color_attr].toString());
  color.setAlphaF(style_dict_[alpha_attr].toDouble());
  QPen* pen = new QPen(color);
  pen->setWidthF(style_dict_[width_attr].toDouble());
  return pen;
}

void SpeedGrapher::AddDataPoint(double y, double progress,
                                const QString& indicator_text) {
  if (progress < 0 || progress > 1) {
    qDebug() << "SpeedGrapher Error: Progress value " << progress
             << " out of range";
    return;
  }
  indicator_text_ = indicator_text;
  ys_.push_back(y);
  progress_ = progress;
  UpdatePlot(true);
}

void SpeedGrapher::AddDataPoint(double y, double progress) {
  AddDataPoint(y, progress, "");
}

void SpeedGrapher::SetProgress(const double progress) {
  progress_ = progress;
}

void SpeedGrapher::SetData(const vector<double>& ys) {
  ys_.assign(ys.begin(), ys.end());
}

double SpeedGrapher::GetDoubleStyleAttribute(StyleAttribute style_attr) {
  return GetStyleAttribute(style_attr).toDouble();
}

QVariant SpeedGrapher::GetStyleAttribute(StyleAttribute style_attr) {
  return style_dict_[style_attr];
}

QString SpeedGrapher::GetStringStyleAttributeS(StyleAttribute style_attr) {
  return GetStyleAttribute(style_attr).toString();
}

const std::vector<double>& SpeedGrapher::GetData() {
  return ys_;
}

void SpeedGrapher::SetStyleAttribute(StyleAttribute style_attr,
                                     const QVariant& style_value) {
  style_dict_[style_attr] = style_value;
}
