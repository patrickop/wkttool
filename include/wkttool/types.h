#pragma once
#include <named_type.hpp>
#include <boost/geometry.hpp>

namespace wkttool {
  
  using Point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;

  using Down = fluent::NamedType<uint32_t, struct DownTag, fluent::Comparable>;
  using Right = fluent::NamedType<uint32_t, struct RightTag, fluent::Comparable>;
  using LowerXBoundary = fluent::NamedType<double, struct LowerXBoundaryTag>;
  using LowerYBoundary = fluent::NamedType<double, struct LowerYBoundaryTag>;
  using UpperXBoundary = fluent::NamedType<double, struct UpperXBoundaryTag>;
  using UpperYBoundary = fluent::NamedType<double, struct UpperYBoundaryTag>;
  using Thickness = fluent::NamedType<double, struct ThicknessTag>;

  struct ScreenLocation {
    Right right;
    Down down;
  };
  struct ScreenDimensions {
    Right right;
    Down down;
  };

  struct CoordinateBoundaries {
    LowerXBoundary lower_x;
    LowerYBoundary lower_y;
    UpperXBoundary upper_x;
    UpperYBoundary upper_y;
  };
  inline double x(const Point& point) {
    return boost::geometry::get<0>(point);
  }
  inline double y(const Point& point) {
    return boost::geometry::get<1>(point);
  }

  namespace shape {
    struct Segment {
      ScreenLocation first;
      ScreenLocation second;

    };
  }

}
