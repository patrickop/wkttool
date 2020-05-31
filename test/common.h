#pragma once
#include <gmock/gmock.h>
#include <wkttool/types.h>

namespace boost::geometry::model {
std::ostream &operator<<(std::ostream &os,
                         const wkttool::geometry::Segment &geo) {
  namespace bg = boost::geometry;
  return os << bg::wkt(geo);
}
}  // namespace boost::geometry::model

MATCHER_P2(SegmentNear, seg, tol, "") {
  namespace bg = boost::geometry;
  return (bg::distance(seg.first, arg.first) < tol and
          bg::distance(seg.second, arg.second) < tol) or
         (bg::distance(seg.first, arg.second) < tol and
          bg::distance(seg.second, arg.first) < tol);
}
MATCHER_P2(PointNear, pt, tol, "") {
  namespace bg = boost::geometry;
  return bg::distance(pt, arg) < tol;
}
