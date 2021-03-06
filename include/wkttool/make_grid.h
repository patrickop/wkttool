#pragma once
#include <wkttool/screen_projection.h>
#include <wkttool/to_drawables.h>
#include <wkttool/types.h>

#include <functional>

namespace wkttool {

geometry::Segment make_vertical_segment(const CoordinateBoundaries& boundaries,
                                        const double x) {
  const auto result =
      geometry::Segment{geometry::Point{x, boundaries.upper_y.get()},
                        geometry::Point{x, boundaries.lower_y.get()}};
  return result;
}
geometry::Segment make_horizontal_segment(
    const CoordinateBoundaries& boundaries, const double y) {
  const auto result =
      geometry::Segment{geometry::Point{boundaries.lower_x.get(), y},
                        geometry::Point{boundaries.upper_x.get(), y}};
  return result;
}

std::vector<geometry::Segment> make_grid(const CoordinateBoundaries& boundaries,
                                         const XStep& x_step,
                                         const YStep& y_step) {
  using namespace std::placeholders;
  namespace rv = ranges::views;
  namespace rng = ranges;
  auto horizontals =
      rv::iota(static_cast<int>(boundaries.lower_y.get() / y_step.get())) |
      rv::transform(std::bind(rng::multiplies{}, y_step.get(), _1)) |
      rv::take_while(
          std::bind(rng::less_equal{}, _1, boundaries.upper_y.get())) |
      rv::transform(std::bind(make_horizontal_segment, boundaries, _1));
  auto verticals =
      rv::iota(static_cast<int>(boundaries.lower_x.get() / x_step.get())) |
      rv::transform(std::bind(rng::multiplies{}, x_step.get(), _1)) |
      rv::take_while(
          std::bind(rng::less_equal{}, _1, boundaries.upper_x.get())) |
      rv::transform(std::bind(make_vertical_segment, boundaries, _1));
  return rv::concat(horizontals, verticals) |
         rng::to<std::vector<geometry::Segment>>;
}

std::vector<geometry::Segment> make_axes(
    const CoordinateBoundaries& boundaries) {
  return {make_horizontal_segment(boundaries, 0),
          make_vertical_segment(boundaries, 0)};
}

}  // namespace wkttool
