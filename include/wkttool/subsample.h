#pragma once
#include <wkttool/types.h>

#include <range/v3/all.hpp>

namespace wkttool {
template <typename Points>
std::vector<geometry::Segment> to_segments(const Points& pts) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  auto froms = pts | rv::drop_last(1);
  auto tos = pts | rv::tail;

  return rv::zip(froms, tos) | rv::transform([](const auto& pair) {
           return geometry::Segment{std::get<0>(pair), std::get<1>(pair)};
         }) |
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
  return to_segments(pts);
}

}  // namespace wkttool
