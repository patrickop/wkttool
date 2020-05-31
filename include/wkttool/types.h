#pragma once
#include <boost/geometry.hpp>
#include <named_type.hpp>
#include <variant>

namespace wkttool {

namespace geometry {
using Point =
    boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
using Segment = boost::geometry::model::segment<Point>;
using Linestring = boost::geometry::model::linestring<Point>;
using Polygon = boost::geometry::model::polygon<Point>;
using MultiPoint = boost::geometry::model::multi_point<Point>;
std::ostream& operator<<(std::ostream& os, const Segment& seg) {
  namespace bg = boost::geometry;
  return os << bg::wkt(seg);
}
}  // namespace geometry

using Down = fluent::NamedType<int, struct DownTag, fluent::Comparable,
                               fluent::Addable, fluent::Subtractable>;
using Right = fluent::NamedType<int, struct RightTag, fluent::Comparable,
                                fluent::Addable, fluent::Subtractable>;
using LowerXBoundary = fluent::NamedType<double, struct LowerXBoundaryTag>;
using LowerYBoundary = fluent::NamedType<double, struct LowerYBoundaryTag>;
using UpperXBoundary = fluent::NamedType<double, struct UpperXBoundaryTag>;
using UpperYBoundary = fluent::NamedType<double, struct UpperYBoundaryTag>;
using Thickness =
    fluent::NamedType<double, struct ThicknessTag, fluent::Comparable>;
using Red = fluent::NamedType<uint8_t, struct RedTag, fluent::Comparable>;
using Green = fluent::NamedType<uint8_t, struct GreenTag, fluent::Comparable>;
using Blue = fluent::NamedType<uint8_t, struct BlueTag, fluent::Comparable>;
using XStep = fluent::NamedType<double, struct XStepTag>;
using YStep = fluent::NamedType<double, struct YStepTag>;
using MouseWheelScollAmount =
    fluent::NamedType<double, struct MouseWheelScrollAmountTag>;

enum class MouseButton { right, left };
struct ScreenLocation {
  Right right;
  Down down;
};
struct ScreenLocationDifference {
  Right right;
  Down down;
};
ScreenLocationDifference operator-(const ScreenLocation& to,
                                   const ScreenLocation& from) {
  return ScreenLocationDifference{to.right - from.right, to.down - from.down};
}
using ScreenLocationPair = std::pair<ScreenLocation, ScreenLocation>;
bool operator==(const ScreenLocation& lhs, const ScreenLocation& rhs) {
  return lhs.down == rhs.down and lhs.right == rhs.right;
}
bool operator==(const ScreenLocationPair& lhs, const ScreenLocationPair& rhs) {
  return lhs.first == rhs.first and lhs.second == rhs.second;
}
std::ostream& operator<<(std::ostream& os, const ScreenLocation& loc) {
  return os << "ScreenLocation "
               " Right: "
            << loc.right.get() << " Down: " << loc.down.get();
}
std::ostream& operator<<(std::ostream& os,
                         const ScreenLocationDifference& loc) {
  return os << "ScreenLocationDifference "
               " Right: "
            << loc.right.get() << " Down: " << loc.down.get();
}
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
inline double x(const geometry::Point& point) {
  return boost::geometry::get<0>(point);
}
inline double y(const geometry::Point& point) {
  return boost::geometry::get<1>(point);
}

template <typename... Ts>
struct TypeList {};
struct WindowClosed {};
struct MouseMoved {
  ScreenLocation destination;
};
struct MouseWheelScrolled {
  ScreenLocation position;
  MouseWheelScollAmount amount;
};
struct MouseButtonDown {
  ScreenLocation location;
  MouseButton button;
};
struct MouseButtonUp {
  ScreenLocation location;
  MouseButton button;
};
using WindowEvent = TypeList<WindowClosed, MouseMoved, MouseWheelScrolled,
                             MouseButtonUp, MouseButtonDown>;
using WindowEventVariant =
    std::variant<WindowClosed, MouseMoved, MouseWheelScrolled, MouseButtonUp,
                 MouseButtonDown>;

struct Color {
  Red red;
  Green green;
  Blue blue;
};
std::ostream& operator<<(std::ostream& os, const Color& color) {
  return os << "Color "
               "Red: "
            << std::to_string(color.red.get())
            << " Green: " << std::to_string(color.green.get())
            << " Blue: " << std::to_string(color.blue.get());
}
bool operator==(const Color& lhs, const Color& rhs) {
  return lhs.red == rhs.red and lhs.green == rhs.green and lhs.blue == rhs.blue;
}
inline constexpr Color black{Red{0}, Green{0}, Blue{0}};
inline constexpr Color white{Red{255}, Green{255}, Blue{255}};
inline constexpr Color grey{Red{200}, Green{200}, Blue{200}};
inline constexpr Color red{Red{255}, Green{0}, Blue{0}};

namespace drawable {
struct Segment {
  Color color;
  Thickness thickness;
  ScreenLocationPair locations;
};
bool operator==(const Segment& lhs, const Segment& rhs) {
  return lhs.color == rhs.color and lhs.thickness == rhs.thickness and
         lhs.locations == rhs.locations;
}
std::ostream& operator<<(std::ostream& os, const Segment& seg) {
  os << "Segment ";
  os << seg.color << " Thickness: " << seg.thickness.get() << " Start: ";
  os << std::get<0>(seg.locations) << " End: ";
  os << std::get<1>(seg.locations);
  return os;
}
}  // namespace drawable

}  // namespace wkttool
