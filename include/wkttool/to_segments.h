#pragma once
#include <wkttool/types.h>

#include <range/v3/all.hpp>

namespace wkttool {
namespace detail {
template <typename Points>
std::vector<geometry::Segment> connect_by_segments(const Points& pts) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  auto froms = pts | rv::drop_last(1);
  auto tos = pts | rv::tail;

  return rv::zip(froms, tos) | rv::transform([](const auto& pair) {
           return geometry::Segment{std::get<0>(pair), std::get<1>(pair)};
         }) |
         rng::to<std::vector<geometry::Segment>>;
}
}  // namespace detail

std::vector<geometry::Segment> to_segments(const geometry::Polygon& polygon) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  const auto inners = polygon.inners();
  using Ring = typename boost::geometry::ring_type<geometry::Polygon>::type;
  const auto inner_drawables =
      inners | rv::transform(detail::connect_by_segments<Ring>) |
      /// Note: this is an action because rangev3 doesnt create temporary
      // containers with transform
      // https://stackoverflow.com/questions/36820639/how-do-i-write-a-range-pipeline-that-uses-temporary-containers
      rng::action::join;
  const auto outer_drawable = detail::connect_by_segments(polygon.outer());

  return rv::concat(inner_drawables, outer_drawable) |
         rng::to<std::vector<geometry::Segment>>;
}

// todo: add concept
std::vector<geometry::Segment> subsample(std::function<double(const double)> fn,
                                         const double lower_x,
                                         const double upper_x,
                                         const int samples) {
  using namespace std::placeholders;
  namespace rv = ranges::views;
  namespace rng = ranges;
  const double samples_db = samples;
  auto xs =
      rv::iota(0, samples) | rv::transform(rng::convert_to<double>{}) |
      rv::transform(
          [&samples_db](const auto& val) { return val / (samples_db - 1); }) |
      rv::transform(
          [diff = upper_x - lower_x](const auto& val) { return val * diff; }) |
      rv::transform([&lower_x](const auto& val) { return val + lower_x; });
  auto pts = xs | rv::transform([&fn](const auto x) {
               return geometry::Point{x, fn(x)};
             });
  return detail::connect_by_segments(pts);
}

}  // namespace wkttool
