#pragma once
#include <cmath>
#include <optional>
#include <wkttool/types.h>

namespace wkttool
{
bool
fits (const Point &point, const CoordinateBoundaries &boundaries)
{
  return x (point) >= boundaries.lower_x.get ()
         and y (point) >= boundaries.lower_y.get ()
         and y (point) <= boundaries.upper_y.get ()
         and x (point) <= boundaries.upper_y.get ();
}
class ScreenProjection
{
public:
  ScreenProjection (const ScreenDimensions &dimensions,
                    const CoordinateBoundaries &boundaries)
      : dimensions{ dimensions }, boundaries{ boundaries }
  {
  }

  std::optional<ScreenLocation>
  to_screen (const Point &point)
  {
    if (not fits (point, boundaries))
      {
        return std::nullopt;
      }
    const auto x_ratio
        = (x (point) - boundaries.lower_x.get ())
          / (boundaries.upper_x.get () - boundaries.lower_x.get ());
    const auto y_ratio
        = (y (point) - boundaries.lower_y.get ())
          / (boundaries.upper_y.get () - boundaries.lower_y.get ());
    const Right right{ static_cast<uint32_t> (
        std::lerp (0, dimensions.right.get (), x_ratio)) };
    const Down down{ static_cast<uint32_t> (
        std::lerp (dimensions.down.get (), 0, y_ratio)) };
    return ScreenLocation{ right, down };
  }
  Point
  to_point (const ScreenLocation &location)
  {
    const auto x_ratio = static_cast<double> (location.right.get ())
                         / dimensions.right.get ();
    const auto y_ratio
        = static_cast<double> (location.down.get ()) / dimensions.down.get ();

    const auto x = std::lerp (boundaries.lower_x.get (),
                              boundaries.upper_x.get (), x_ratio);
    const auto y = std::lerp (boundaries.upper_y.get (),
                              boundaries.lower_y.get (), y_ratio);
    return Point{ x, y };
  }

private:
  ScreenDimensions dimensions;
  CoordinateBoundaries boundaries;
};

}
