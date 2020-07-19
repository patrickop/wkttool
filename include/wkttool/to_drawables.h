#pragma once
#include <wkttool/subsample.h>
#include <wkttool/types.h>

#include <range/v3/all.hpp>
namespace wkttool {
namespace detail {
template <typename Projection>
drawable::Segment to_drawable(const geometry::Segment& geo, const Color& color,
                              const Thickness& thickness,
                              Projection& projector) {
  return drawable::Segment{color, thickness, projector.to_screen(geo)};
}
}  // namespace detail
// Todo: add concept
template <typename Projection, typename Segments>
std::vector<drawable::Segment> segments_to_drawables(
    const Segments& segments, const Projection& projector,
    const Color& lines_color, const Thickness& thickness) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  using namespace std::placeholders;
  return segments |
         rv::transform(std::bind(detail::to_drawable<Projection>, _1,
                                 lines_color, thickness, projector)) |
         rng::to<std::vector<drawable::Segment>>;
}
// Todo: add concept
template <typename Projection>
std::vector<drawable::Segment> point_to_drawables(const geometry::Point& point,
                                                  const Projection& projector,
                                                  const Color& lines_color,
                                                  const Thickness& thickness,
                                                  const Right& right_leg,
                                                  const Down& down_leg) {
  const auto neg_right_leg = Right{0} - right_leg;
  const auto neg_down_leg = Down{0} - down_leg;
  const auto top_left = projector.translate(
      point, ScreenLocationDifference{neg_right_leg, neg_down_leg});
  const auto top_right = projector.translate(
      point, ScreenLocationDifference{right_leg, neg_down_leg});
  const auto bottom_left = projector.translate(
      point, ScreenLocationDifference{neg_right_leg, down_leg});
  const auto bottom_right =
      projector.translate(point, ScreenLocationDifference{right_leg, down_leg});
  std::vector<geometry::Segment> segments{
      geometry::Segment{top_left, bottom_right},
      geometry::Segment{bottom_left, top_right}};
  return segments_to_drawables(segments, projector, lines_color, thickness);
}

template <typename Projection>
std::vector<drawable::Segment> points_to_drawables(const geometry::MultiPoint& points,
                                                  const Projection& projector,
                                                  const Color& lines_color,
                                                  const Thickness& thickness,
                                                  const Right& right_leg,
                                                  const Down& down_leg) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  const auto to_drawables = [&] (const auto& pt) {
    return point_to_drawables(pt, projector, lines_color, thickness, right_leg, down_leg); };
  return points | rv::transform(to_drawables) | rng::action::join | 
         rng::to<std::vector<drawable::Segment>>;

}


}  // namespace wkttool
