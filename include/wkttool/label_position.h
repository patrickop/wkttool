#pragma once
#include <wkttool/types.h>

namespace wkttool {
template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

geometry::Point label_position(const geometry::Geometry& geo) {
  return std::visit(
      overload{[](const geometry::Point& point) { return point; },
               [](const geometry::MultiPoint& mp) { return mp[0]; },
               [](const geometry::Linestring& ls) { return ls[0]; },
               [](const geometry::MultiLinestring& mls) { return mls[0][0]; },
               [](const geometry::Polygon& poly) { return poly.outer()[0]; },
               [](const geometry::MultiPolygon& polys) {
                 return polys[0].outer()[0];
               },
               [](const auto&) {
                 return geometry::Point{0, 0};
               }},
      geo);
}
}  // namespace wkttool
