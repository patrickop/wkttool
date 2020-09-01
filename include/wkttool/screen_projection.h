#pragma once
#include <wkttool/types.h>

#include <cmath>

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

  ScreenLocation to_screen(const geometry::Point &point) const {
    const auto mapped_x = range_map_x(x(point));
    const auto mapped_y = range_map_y(y(point));
    const Right right{mapped_x};
    const Down down{mapped_y};
    return ScreenLocation{right, down};
  }

  geometry::Point to_point(const ScreenLocation &location) const {
    const auto x_per_point =
        (boundaries.upper_x.get() - boundaries.lower_x.get()) /
        static_cast<double>(dimensions.right.get());
    const auto y_per_point =
        (boundaries.upper_y.get() - boundaries.lower_y.get()) /
        static_cast<double>(dimensions.down.get());

    return geometry::Point{
        boundaries.lower_x.get() + x_per_point * location.right.get(),
        boundaries.upper_y.get() - y_per_point * location.down.get()};
  }

  ScreenLocationPair to_screen(const geometry::Segment &segment) const {
    return ScreenLocationPair{to_screen(std::get<0>(segment)),
                              to_screen(std::get<1>(segment))};
  }
  // TOdo: refactor
  std::array<double, 2> to_coords_difference(
      const ScreenLocationDifference &difference) const {
    const auto x_per_point =
        (boundaries.upper_x.get() - boundaries.lower_x.get()) /
        static_cast<double>(dimensions.right.get());
    const auto y_per_point =
        (boundaries.lower_y.get() - boundaries.upper_y.get()) /
        static_cast<double>(dimensions.down.get());

    return std::array<double, 2>{x_per_point * difference.right.get(),
                                 y_per_point * difference.down.get()};
  }

  geometry::Point translate(const geometry::Point &base,
                            const ScreenLocationDifference &difference) const {
    namespace trans = boost::geometry::strategy::transform;
    namespace bg = boost::geometry;
    // todo: this is ugly
    const auto coords_difference = to_coords_difference(difference);
    trans::translate_transformer<double, 2, 2> translate(
        std::get<0>(coords_difference), std::get<1>(coords_difference));
    geometry::Point result;
    boost::geometry::transform(base, result, translate);

    return result;
  }

 private:
  geometry::Polygon get_screen_polygon() const {
    return geometry::Polygon{
        {geometry::Point{0, 0},
         geometry::Point{static_cast<double>(dimensions.right.get() - 1), 0},
         geometry::Point{static_cast<double>(dimensions.right.get() - 1),
                         static_cast<double>(dimensions.down.get() - 1)},
         geometry::Point{0, static_cast<double>(dimensions.down.get() - 1)},
         geometry::Point{0, 0}}};
  }
  int range_map_x(double x) const {
    return range_map_discrete(boundaries.lower_x.get(),
                              boundaries.upper_x.get(), 0,
                              dimensions.right.get(), x);
  }
  int range_map_y(double y) const {
    return range_map_discrete(boundaries.lower_y.get(),
                              boundaries.upper_y.get(), dimensions.down.get(),
                              0, y);
  }

  geometry::Point scale_to_screen(const geometry::Point &point) const {
    const auto mapped_x = range_map_x(x(point));
    const auto mapped_y = range_map_y(y(point));
    return geometry::Point{static_cast<double>(mapped_x),
                           static_cast<double>(mapped_y)};
  }
  ScreenDimensions dimensions;
  CoordinateBoundaries boundaries;
};

}  // namespace wkttool
