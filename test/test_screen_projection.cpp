#include <gmock/gmock.h>
#include <wkttool/screen_projection.h>

namespace boost::geometry::model
{
std::ostream &
operator<< (std::ostream &os, const wkttool::Point &p)
{
  return os << "Point {x " << get<0> (p) << ", y " << get<1> (p) << "}";
}
}

namespace wkttool
{
bool
operator== (const ScreenLocation &lhs, const ScreenLocation &rhs)
{
  return lhs.right == rhs.right and lhs.down == rhs.down;
}
std::ostream &
operator<< (std::ostream &os, const ScreenLocation &loc)
{
  return os << "Screen Location {Right " << loc.right.get () << ", Down "
            << loc.down.get () << "}";
}
}
using namespace wkttool;
MATCHER_P2 (Near, expected, tolerance, "")
{
  namespace bg = boost::geometry;
  return bg::distance (expected, arg) < tolerance;
}
using ::testing::Optional;

TEST (TestScreenProjection, ProjectsPointInsideScreen)
{
  ScreenDimensions dim{ Right{ 200 }, Down{ 200 } };
  CoordinateBoundaries bound{ LowerXBoundary{ -1.0 }, LowerYBoundary{ -1.0 },
                              UpperXBoundary{ 1.0 }, UpperYBoundary{ 1.0 } };
  ScreenProjection projection{ dim, bound };
  Point p{ 0.5, 0.2 };
  ScreenLocation expected_position{ Right{ 150 }, Down{ 80 } };
  const auto sp = projection.to_screen (p);

  EXPECT_THAT (sp, Optional (expected_position));
}
TEST (TestScreenProjection, DoesntProjectPointOutsideScreen)
{
  ScreenDimensions dim{ Right{ 200 }, Down{ 200 } };
  CoordinateBoundaries bound{ LowerXBoundary{ -1.0 }, LowerYBoundary{ -1.0 },
                              UpperXBoundary{ 1.0 }, UpperYBoundary{ 1.0 } };
  ScreenProjection projection{ dim, bound };
  Point p{ 1.5, 0.2 };
  ScreenLocation expected_position{ Right{ 150 }, Down{ 80 } };
  const auto sp = projection.to_screen (p);

  EXPECT_THAT (sp, std::nullopt);
}
TEST (TestScreenProjection, InterpolatesPoint)
{
  ScreenDimensions dim{ Right{ 200 }, Down{ 200 } };
  CoordinateBoundaries bound{ LowerXBoundary{ -1.0 }, LowerYBoundary{ -1.0 },
                              UpperXBoundary{ 1.0 }, UpperYBoundary{ 1.0 } };
  ScreenProjection projection{ dim, bound };
  ScreenLocation loc{ Right{ 20 }, Down{ 130 } };
  Point expected_point{ -0.8, -0.3 };
  const auto p = projection.to_point (loc);

  EXPECT_THAT (p, Near (expected_point, 1e-5));
}
int
main (int argc, char **argv)
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}
