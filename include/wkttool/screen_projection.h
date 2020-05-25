#pragma once
#include <wkttool/types.h>

#include <cmath>
#include <optional>

namespace wkttool {
/// puts a number into the fitting bucket of integers
/// for example
/// values >= 10 and < 11 are put into the "10" bucket
/// values >= 11 and < 12 are put into the "11" bucket
/// etc.
/// tolerance is added to the value before the bucket check is made
int make_discrete(const double value, const double tolerance = 1e-5) {
  return std::floor(value + tolerance);
}

int range_map_discrete(const double source_range_lower,
                       const double source_range_upper,
                       const int target_range_lower,
                       const int target_range_upper,
                       const double source_range_value) {
  const auto ratio = (source_range_value - source_range_lower) /
                     (source_range_upper - source_range_lower);
  const auto interpolated =
      std::lerp(target_range_lower, target_range_upper, ratio);

  const auto discrete = make_discrete(interpolated);
  return discrete;
}

class ScreenProjection {
 public:
  ScreenProjection(const ScreenDimensions &dimensions,
                   const CoordinateBoundaries &boundaries)
      : dimensions{dimensions}, boundaries{boundaries} {}

  std::optional<ScreenLocation> to_screen(const geometry::Point &point) {
    const auto mapped_x = range_map_x(x(point));
    if (mapped_x < 0 or mapped_x >= dimensions.right.get()) {
      return std::nullopt;
    }
    const auto mapped_y = range_map_y(y(point));
    if (mapped_y < 0 or mapped_y >= dimensions.down.get()) {
      return std::nullopt;
    }
    const Right right{static_cast<uint32_t>(mapped_x)};
    const Down down{static_cast<uint32_t>(mapped_y)};
    return ScreenLocation{right, down};
  }
  std::optional<ScreenLocationPair> to_screen(
      const geometry::Segment &segment) {
    namespace bg = boost::geometry;
    // intersections does not  support segment (yet)
    geometry::Linestring segment_ls{scale_to_screen(segment.first),
                                    scale_to_screen(segment.second)};
    geometry::Polygon screen = {
        {geometry::Point{0, 0},
         geometry::Point{static_cast<double>(dimensions.right.get() - 1), 0},
         geometry::Point{static_cast<double>(dimensions.right.get() - 1),
                         static_cast<double>(dimensions.down.get() - 1)},
         geometry::Point{0, static_cast<double>(dimensions.down.get() - 1)},
         geometry::Point{0, 0}}};

    geometry::MultiPoint out;
    bg::intersection(screen, segment_ls, out);

    ScreenLocation first{Right{0}, Down{0}}, second{Right{0}, Down{0}};
    const auto trivial_first = to_screen(std::get<0>(segment));
    const auto trivial_second = to_screen(std::get<1>(segment));

    // todo: return nullopt
    if (trivial_first) {
      first = *trivial_first;
    } else {
      const geometry::Point first_p = segment_ls[0];
      const auto closest = std::min_element(
          std::begin(out), std::end(out),
          [first_p](const geometry::Point &lhs, const geometry::Point &rhs) {
            return bg::distance(first_p, lhs) < bg::distance(first_p, rhs);
          });
      first = ScreenLocation{Right{make_discrete(x(*closest))},
                             Down{make_discrete(y(*closest))}};
    }

    if (trivial_second) {
      second = *trivial_second;
    } else {
      const geometry::Point second_p = segment_ls[1];
      const auto closest = std::min_element(
          std::begin(out), std::end(out),
          [second_p](const geometry::Point &lhs, const geometry::Point &rhs) {
            return bg::distance(second_p, lhs) < bg::distance(second_p, rhs);
          });
      second = ScreenLocation{Right{make_discrete(x(*closest))},
                              Down{make_discrete(y(*closest))}};
    }

    return ScreenLocationPair{first, second};
  }
  geometry::Point to_point(const ScreenLocation &location) {
    const auto x_ratio =
        static_cast<double>(location.right.get()) / dimensions.right.get();
    const auto y_ratio =
        static_cast<double>(location.down.get()) / dimensions.down.get();

    const auto x =
        std::lerp(boundaries.lower_x.get(), boundaries.upper_x.get(), x_ratio);
    const auto y =
        std::lerp(boundaries.upper_y.get(), boundaries.lower_y.get(), y_ratio);
    return geometry::Point{x, y};
  }

 private:
  int range_map_x(double x) {
    return range_map_discrete(boundaries.lower_x.get(),
                              boundaries.upper_x.get(), 0,
                              dimensions.right.get(), x);
  }
  int range_map_y(double y) {
    return range_map_discrete(boundaries.lower_y.get(),
                              boundaries.upper_y.get(), dimensions.down.get(),
                              0, y);
  }

  geometry::Point scale_to_screen(const geometry::Point &point) {
    const auto mapped_x = range_map_x(x(point));
    const auto mapped_y = range_map_y(y(point));
    return geometry::Point{mapped_x, mapped_y};
  }
  ScreenDimensions dimensions;
  CoordinateBoundaries boundaries;
};

}  // namespace wkttool
