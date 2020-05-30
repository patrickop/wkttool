#pragma once
#include <wkttool/subsample.h>
#include <wkttool/types.h>

#include <range/v3/all.hpp>
namespace wkttool {
namespace detail {
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
}  // namespace detail
// TODO: unit test
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
         rv::filter([](const auto& seg) { return seg != std::nullopt; }) |
         rv::transform([](const auto& seg) { return seg.value(); }) |
         rng::to<std::vector<drawable::Segment>>;
}

}  // namespace wkttool
