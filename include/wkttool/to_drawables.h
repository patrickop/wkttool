#pragma once
#include <wkttool/types.h>

#include <range/v3/all.hpp>
namespace wkttool {
// TODO: unit test
std::optional<drawable::Segment> to_drawable(const geometry::Segment& geo,
                                             const Color& color,
                                             const Thickness& thickness,
                                             ScreenProjection& projector) {
  const auto loc = projector.to_screen(geo);
  if (loc)
    return drawable::Segment{color, thickness, *loc};
  else
    return std::nullopt;
}
template <typename Segments>
std::vector<drawable::Segment> to_drawables(const Segments& segments,
                                            const ScreenProjection& projector,
                                            const Color& lines_color,
                                            const Thickness& thickness) {
  namespace rv = ranges::views;
  namespace rng = ranges;
  using namespace std::placeholders;
  return segments |
         rv::transform(
             std::bind(to_drawable, _1, lines_color, thickness, projector)) |
         rv::filter([](const auto& seg) { return seg != std::nullopt; }) |
         rv::transform([](const auto& seg) { return seg.value(); }) |
         rng::to<std::vector<drawable::Segment>>;
}
}  // namespace wkttool
