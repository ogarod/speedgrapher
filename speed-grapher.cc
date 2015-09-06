#include "speed-grapher.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <QGraphicsTextItem>
#include <QDebug>

using std::string;
using std::vector;
using std::unique_ptr;
using std::stringstream;
using std::unordered_map;

static const char* kUmemeStyle =
    "QGroupBox { "
    "  border: 0; "
    "}"
    "QGraphicsView { "
    "  border: 1px solid #ddd; "
    "}";

SpeedGrapher::SpeedGrapher(QWidget* parent, const QSize& size,
                           int num_vertical_gridlines,
                           int num_horizontal_gridlines)
    : QGraphicsView(parent) {
  size_ = size;
  num_vertical_gridlines_ = num_vertical_gridlines;
  num_horizontal_gridlines_ = num_horizontal_gridlines;
  style_dict_ = {
      {"background_brush_color", "#fff"},
      {"background_brush_alpha", 1.0},
      {"grid_pen_color", "#ccc"},
      {"grid_pen_alpha", 0.15},
      {"grid_pen_width", 0.0},
      {"curve_pen_color", "#060"},
      {"curve_pen_alpha", 1.0},
      {"curve_pen_width", 0.0},
      {"curve_brush_color", "#0f0"},
      {"curve_brush_alpha", 0.5},
      {"chart_brush_color", "#0f0"},
      {"chart_brush_alpha", 0.33},
      {"indicator_dot_pen_color", "#ccc"},
      {"indicator_dot_pen_alpha", 1.0},
      {"indicator_dot_pen_width", 0.0},
      {"indicator_dot_brush_color", "#0f0"},
      {"indicator_dot_brush_alpha", 0.5},
      {"indicator_dot_size", 8.0},
      {"indicator_line_pen_color", "#ccc"},
      {"indicator_line_pen_alpha", 1.0},
      {"indicator_line_pen_width", 0.0},
      {"indicator_text_font_name", "Verdana"},
      {"indicator_text_font_size", 10.0},
      {"indicator_text_color", "#ccc"},
      {"indicator_text_alpha", 1.0},
  };
  ys_ = {0};
  yprop_ = 0.83;
  progress_ = 0;
  indicator_text_margin_bottom_ = 3;
  indicator_text_margin_right_ = 10;

  setScene(&scene_);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  scene_.setSceneRect(0, 0, size_.width(), size_.height());
  setFrameShape(QFrame::NoFrame);
  setFixedSize(size_);
  UpdatePlot(false);
  setStyleSheet(kUmemeStyle);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  show();
}

void SpeedGrapher::SetProgress(const double progress) {
  progress_ = progress;
}

const QVariant& SpeedGrapher::GetStyleAttribute(const QString& style_attr) {
  return style_dict_[style_attr];
}

void SpeedGrapher::SetStyleAttribute(const QString& style_attr,
                                     const QVariant& style_value) {
  style_dict_[style_attr] = style_value;
}

void SpeedGrapher::UpdatePlot(bool show_indicators) {
  scene_.clear();

  // Draw gridlines.
  unique_ptr<QBrush> brush = MkBrush("background");
  scene_.setBackgroundBrush(*brush);
  DrawGridlines();

  // Draw curve
  double last_x = 0;
  double last_y = 0;
  unique_ptr<QPainterPath> path(ComputeSeriesPath(&last_x, &last_y));
  scene_.addPath(*path, *MkPen("curve"));

  // Shade area under the curve.
  QPainterPath area(*path);
  area.lineTo(last_x, size_.height());
  area.closeSubpath();
  scene_.addPath(area, Qt::NoPen, *MkBrush("curve"));

  // Shade area behind the progress level.
  scene_.addRect(0, 0, progress_ * size_.width(), size_.height(),
                 Qt::NoPen, *MkBrush("chart"));

  // If we are not supposed to show the indicators, return here.
  if (!show_indicators) {
    return;
  }
  // std::cout << progress_ << std::endl;
  // Draw indicator dot.
  double dot_size = style_dict_["indicator_dot_size"].toDouble();
  double dot_x = last_x - dot_size / 2;
  double dot_y = last_y - dot_size / 2;
  scene_.addEllipse(dot_x, dot_y, dot_size, dot_size,
                    *MkPen("indicator_dot"),
                    *MkBrush("indicator_dot"));

  // Draw horizontal indicator line.
  scene_.addLine(0, last_y+0.5, size_.width(), last_y+0.5,
                 *MkPen("indicator_line"));

  // Draw a caption for the horizontal line.
  QString text = indicator_text_;
  if (text.isEmpty()) {
    text = QString::number(last_y);
  }
  QGraphicsTextItem* text_item = scene_.addText(
        text,
        QFont(style_dict_["indicator_text_font_name"].toString(),
              style_dict_["indicator_text_font_size"].toDouble()));
  QColor text_color = QColor();
  text_color.setNamedColor(style_dict_["indicator_text_color"].toString());
  text_color.setAlphaF(style_dict_["indicator_text_alpha"].toDouble());
  text_item->setDefaultTextColor(text_color);
  double text_x = (size_.width() - indicator_text_margin_right_ -
                   text_item->boundingRect().width());
  double text_y = (last_y - indicator_text_margin_bottom_ -
                   text_item->boundingRect().height());
  text_item->setPos(text_x, text_y < 0 ? 0 : text_y);
}

void SpeedGrapher::DrawGridlines() {
  unique_ptr<QPen> grid_pen = MkPen("grid");

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

unique_ptr<QBrush> SpeedGrapher::MkBrush(const QString& brush_name) {
  QColor color;
  const QString color_attr = QString("%1_brush_color").arg(brush_name);
  const QString alpha_attr = QString("%1_brush_alpha").arg(
      brush_name);
  const QString& named_color = style_dict_[color_attr].toString();
  color.setNamedColor(named_color);
  color.setAlphaF(style_dict_[alpha_attr].toDouble());
  return unique_ptr<QBrush>(new QBrush(color));
}

unique_ptr<QPen> SpeedGrapher::MkPen(const QString& pen_name) {
  QColor color;
  const QString color_attr = QString("%1_pen_color").arg(pen_name);
  const QString alpha_attr = QString("%1_pen_alpha").arg(pen_name);
  const QString width_attr = QString("%1_pen_width").arg(pen_name);
  color.setNamedColor(style_dict_[color_attr].toString());
  color.setAlphaF(style_dict_[alpha_attr].toDouble());
  unique_ptr<QPen> pen(new QPen(color));
  pen->setWidthF(style_dict_[width_attr].toDouble());
  return pen;
}

void SpeedGrapher::AddDataPoint(double y, double progress,
                                const QString& indicator_text) {
  if (progress < 0 || progress > 1) {
    // TODO(ogaro): Log message.
    // http://doc.qt.io/qt-5/qtglobal.html#qInstallMessageHandler
    qDebug() << "Progress value " << progress << " out of range";
    return;
  }
  indicator_text_ = indicator_text;
  ys_.push_back(y);
  progress_ = progress;
  UpdatePlot(true);
}
