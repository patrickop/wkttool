#pragma once
#include <boost/geometry.hpp>
#include <named_type.hpp>
#include <variant>

namespace wkttool
{

using Point
    = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;

using Down = fluent::NamedType<uint32_t, struct DownTag, fluent::Comparable>;
using Right = fluent::NamedType<uint32_t, struct RightTag, fluent::Comparable>;
using LowerXBoundary = fluent::NamedType<double, struct LowerXBoundaryTag>;
using LowerYBoundary = fluent::NamedType<double, struct LowerYBoundaryTag>;
using UpperXBoundary = fluent::NamedType<double, struct UpperXBoundaryTag>;
using UpperYBoundary = fluent::NamedType<double, struct UpperYBoundaryTag>;
using Thickness = fluent::NamedType<double, struct ThicknessTag>;
using Red = fluent::NamedType<uint8_t, struct RedTag>;
using Green = fluent::NamedType<uint8_t, struct GreenTag>;
using Blue = fluent::NamedType<uint8_t, struct BlueTag>;

struct ScreenLocation
{
  Right right;
  Down down;
};
struct ScreenDimensions
{
  Right right;
  Down down;
};

struct CoordinateBoundaries
{
  LowerXBoundary lower_x;
  LowerYBoundary lower_y;
  UpperXBoundary upper_x;
  UpperYBoundary upper_y;
};
inline double
x (const Point &point)
{
  return boost::geometry::get<0> (point);
}
inline double
y (const Point &point)
{
  return boost::geometry::get<1> (point);
}

template <typename... Ts> struct TypeList
{
};
struct WindowClosed
{
};
struct MouseMoved
{
  ScreenLocation destination;
};
using WindowEvent = TypeList<WindowClosed, MouseMoved>;
using WindowEventVariant = std::variant<WindowClosed, MouseMoved>;

struct Color
{
  Red red;
  Green green;
  Blue blue;
};
inline constexpr Color black{ Red{ 0 }, Green{ 0 }, Blue{ 0 } };
inline constexpr Color white{ Red{ 255 }, Green{ 255 }, Blue{ 255 } };

namespace shape
{
struct Segment
{
  Color color;
  Thickness thickness;
  ScreenLocation start;
  ScreenLocation end;
};
}

}
