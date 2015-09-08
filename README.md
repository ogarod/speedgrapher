# SpeedGrapher
A Qt widget for real-time visualization of speed and progress. SpeedGrapher is the combination of a line chart and a progress bar.
![Speed Grapher screenshot 1]
(https://github.com/demonchild2112/speedgrapher/blob/master/images/sg_shot1.png)

## Usage
Using the widget is pretty straightforward. First, instantiate a SpeedGrapher. You have three constructors to pick from. The base constructor allows you to specify a size for the widget and the number of vertical and horizontal gridlines to show. The other constructors define defaults that may or may not be suited to your needs.
```cpp
#include "speed-grapher.h"
...
SpeedGrapher* speed_grapher = new SpeedGrapher(this); // Default size and gridline count.
```
While the operation you want to visualize is in progress, call  `AddDataPoint()` to add data points to the SpeedGrapher. Usually, you would do this within the slot of a `QTimer`'s `timeout()` signal. The SpeedGrapher updates the plot whenever a new data point is added. You have the option of specifying a caption that is displayed along with the horizontal indicator line (if none is given, the data point that was just added is used as the caption).
```cpp
speed_grapher->AddDataPoint(data_point, progress, caption); // progress must be between 0 and 1.
```
The color theme for the SpeedGrapher is highly customizable. You use `SetStyleAttribute()` to change particular style attributes for a SpeedGrapher instance. The default style attributes are defined in `speedgrapher.cc`
```cpp
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
```
That's it! Clone the repository and play with the demo.
