#pragma once
#include <wkttool/subsample.h>
#include <wkttool/types.h>

#include <range/v3/all.hpp>
namespace wkttool {
// TODO: unit test
// Todo: add concept
template <typename Projection>
std::optional<drawable::Segment> to_drawable(const geometry::Segment& geo,
                                             const Color& color,
                                             const Thickness& thickness,
                                             Projection& projector) {
  const auto loc = projector.to_screen(geo);
  if (loc)
    return drawable::Segment{color, thickness, *loc};
  else
    return std::nullopt;
}
template <typename Projection, typename Segments>
std::vector<drawable::Segment> segments_to_drawables(
    const Segments& segments, const Projection& projector,
    const Color& lines_color, const Thickness& thickness) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  using namespace std::placeholders;
  return segments |
         rv::transform(std::bind(to_drawable<Projection>, _1, lines_color,
                                 thickness, projector)) |
         rv::filter([](const auto& seg) { return seg != std::nullopt; }) |
         rv::transform([](const auto& seg) { return seg.value(); }) |
         rng::to<std::vector<drawable::Segment>>;
}
template <typename Ring, typename Projection>
std::vector<drawable::Segment> ring_to_drawables(const Ring& ring,
                                                 const Projection& projector,
                                                 const Color& lines_color,
                                                 const Thickness& thickness) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  using namespace std::placeholders;
  auto segments = to_segments(ring);
  return segments |
         rv::transform(
             [&](const auto& seg) -> std::optional<drawable::Segment> {
               if (const auto loc = projector.to_screen(seg); loc) {
                 return drawable::Segment{lines_color, thickness, *loc};
               } else {
                 return std::nullopt;
               }
             }) |
         rv::filter([](const auto& seg) { return seg != std::nullopt; }) |
         rv::transform([](const auto& seg) { return seg.value(); }) |
         rng::to<std::vector<drawable::Segment>>;
}
template <typename Projection>
std::vector<drawable::Segment> polygon_to_drawables(
    const geometry::Polygon& polygon, const Projection& projector,
    const Color& lines_color, const Thickness& thickness) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  using namespace std::placeholders;
  std::vector<boost::geometry::ring_type<geometry::Polygon>::type> inners =
      polygon.inners();
  const auto inner_drawables =
      inners | rv::transform([&](const auto& ring) {
        return ring_to_drawables(ring, projector, lines_color, thickness);
      }) |
      /// Note: this is an action because rangev3 doesnt create temporary
      // containers with transform
      // https://stackoverflow.com/questions/36820639/how-do-i-write-a-range-pipeline-that-uses-temporary-containers
      rng::action::join;
  const auto outer_drawable =
      ring_to_drawables(polygon.outer(), projector, lines_color, thickness);

  return rv::concat(inner_drawables, outer_drawable) |
         rng::to<std::vector<drawable::Segment>>;
}
}  // namespace wkttool
