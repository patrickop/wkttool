#pragma once
#include <wkttool/screen_projection.h>
#include <wkttool/types.h>

#include <functional>
#include <range/v3/all.hpp>

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

auto make_grid_coords(const CoordinateBoundaries& boundaries,
                      const XStep& x_step, const YStep& y_step) {
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
  return rv::concat(horizontals, verticals);
}

std::optional<shape::Segment> to_shape(const geometry::Segment& geo,
                                       const Color& color,
                                       const Thickness& thickness,
                                       ScreenProjection& projector) {
  const auto loc = projector.to_screen(geo);
  if (loc)
    return shape::Segment{color, thickness, *loc};
  else
    return std::nullopt;
}
template <typename Segments>
std::vector<shape::Segment> to_shapes(const Segments& segments,
                                      const ScreenProjection& projector,
                                      const Color& lines_color,
                                      const Thickness& thickness) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  using namespace std::placeholders;
  return segments |
         rv::transform(
             std::bind(to_shape, _1, lines_color, thickness, projector)) |
         rv::filter([](const auto& seg) { return seg != std::nullopt; }) |
         rv::transform([](const auto& seg) { return seg.value(); }) |
         rng::to<std::vector<shape::Segment>>;
}
std::vector<shape::Segment> make_grid(const CoordinateBoundaries& boundaries,
                                      const ScreenProjection& projector,
                                      const XStep& x_step, const YStep& y_step,
                                      const Color& lines_color) {
  return to_shapes(make_grid_coords(boundaries, x_step, y_step), projector,
                   lines_color, Thickness{1});
}

std::vector<shape::Segment> make_axes(const CoordinateBoundaries& boundaries,
                                      const ScreenProjection& projector,
                                      const Color& lines_color) {
  std::vector<geometry::Segment> axes{make_horizontal_segment(boundaries, 0),
                                      make_vertical_segment(boundaries, 0)};
  return to_shapes(axes, projector, lines_color, Thickness{1});
}

}  // namespace wkttool
