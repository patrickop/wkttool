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
      rv::take_while(std::bind(rng::less_equal{}, _1, boundaries.upper_y.get())) |
      rv::transform(std::bind(make_horizontal_segment, boundaries, _1));
  auto verticals =
      rv::iota(static_cast<int>(boundaries.lower_x.get() / x_step.get())) |
      rv::transform(std::bind(rng::multiplies{}, x_step.get(), _1)) |
      rv::take_while(std::bind(rng::less_equal{}, _1, boundaries.upper_x.get())) |
      rv::transform(std::bind(make_vertical_segment, boundaries, _1));
  return rv::concat(horizontals, verticals);
}

std::optional<shape::Segment> to_shape(const geometry::Segment& geo, const Color& color,
                        const Thickness& thickness,
                        ScreenProjection& projector) {
  const auto loc = projector.to_screen(geo);
  if (loc)
    return shape::Segment{color, thickness, *loc};
  else 
    return std::nullopt;
}

std::vector<shape::Segment> make_grid(const CoordinateBoundaries& boundaries,
                                      const ScreenProjection& projector,
                                      const XStep& x_step, const YStep& y_step,
                                      const Color& main_lines_color,
                                      const Color& supplementary_lines_color) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  using namespace std::placeholders;
  namespace bg = boost::geometry;

  const auto grid_coords = make_grid_coords(boundaries, x_step, y_step);
  const auto is_main_line = [](const auto& seg) {
    return bg::distance(geometry::Point{0, 0}, seg) < 1e-5;
  };

  auto main_lines = grid_coords | rv::filter(is_main_line) |
                    rv::transform(std::bind(to_shape, _1, main_lines_color,
                                            Thickness{1}, projector));
  auto supplementary_lines =
      grid_coords | rv::filter(rng::not_fn(is_main_line)) |
      rv::transform(std::bind(to_shape, _1, supplementary_lines_color,
                              Thickness{1}, projector));

  return rv::concat(main_lines, supplementary_lines) |
         rv::filter([] (const auto& seg) {return seg != std::nullopt;}) |
         rv::transform([] (const auto& seg) {return seg.value();}) |
         rng::to<std::vector<shape::Segment>>;
}

}  // namespace wkttool
