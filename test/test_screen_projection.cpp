#include <gmock/gmock.h>
#include <wkttool/screen_projection.h>

namespace boost::geometry::model {
std::ostream &operator<<(std::ostream &os,
                         const wkttool::geometry::Point &geo) {
  namespace bg = boost::geometry;
  return os << bg::wkt(geo);
}
}  // namespace boost::geometry::model

using namespace wkttool;
MATCHER_P2(Near, expected, tolerance, "") {
  namespace bg = boost::geometry;
  return bg::distance(expected, arg) < tolerance;
}
using ::testing::Optional;

TEST(TestScreenProjection, ProjectsPointInsideScreen) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-100.0}, LowerYBoundary{-100.0},
                             UpperXBoundary{100.0}, UpperYBoundary{100.0}};
  ScreenProjection projection{dim, bound};
  geometry::Point p{50, 20};
  ScreenLocation expected_position{Right{150}, Down{80}};
  const auto sp = projection.to_screen(p);

  EXPECT_THAT(sp, Optional(expected_position));
}
TEST(TestScreenProjection, ProjectsPointInsideScreenRounding) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-100.0}, LowerYBoundary{-100.0},
                             UpperXBoundary{100.0}, UpperYBoundary{100.0}};
  ScreenProjection projection{dim, bound};
  geometry::Point p{50.5, 20.9};
  ScreenLocation expected_position{Right{150}, Down{79}};
  const auto sp = projection.to_screen(p);

  EXPECT_THAT(sp, Optional(expected_position));
}
TEST(TestScreenProjection, ProjectsPointInsideScreenLowerEdge) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-100.0}, LowerYBoundary{-100.0},
                             UpperXBoundary{100.0}, UpperYBoundary{100.0}};
  ScreenProjection projection{dim, bound};
  geometry::Point p{-100, 100};
  ScreenLocation expected_position{Right{0}, Down{0}};
  const auto sp = projection.to_screen(p);

  EXPECT_THAT(sp, Optional(expected_position));
}
TEST(TestScreenProjection, ProjectsPointInsideScreenUpperEdge) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-100.0}, LowerYBoundary{-100.0},
                             UpperXBoundary{100.0}, UpperYBoundary{100.0}};
  ScreenProjection projection{dim, bound};
  geometry::Point p{99, -99};
  ScreenLocation expected_position{Right{199}, Down{199}};
  const auto sp = projection.to_screen(p);

  EXPECT_THAT(sp, Optional(expected_position));
}
TEST(TestScreenProjection, ProjectsPointOutsideScreenLowerEdge) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-100.0}, LowerYBoundary{-100.0},
                             UpperXBoundary{100.0}, UpperYBoundary{100.0}};
  ScreenProjection projection{dim, bound};
  geometry::Point p{-101, 101};
  const auto sp = projection.to_screen(p);

  ScreenLocation expected_position{Right{-1}, Down{-1}};
  EXPECT_THAT(sp, Optional(expected_position));
}
TEST(TestScreenProjection, ProjectsPointOutsideScreenUpperEdge) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-100.0}, LowerYBoundary{-100.0},
                             UpperXBoundary{100.0}, UpperYBoundary{100.0}};
  ScreenProjection projection{dim, bound};
  geometry::Point p{100, -100};
  const auto sp = projection.to_screen(p);

  ScreenLocation expected_position{Right{200}, Down{200}};
  EXPECT_THAT(sp, Optional(expected_position));
}
TEST(TestScreenProjection, ProjectsFullyCoveredSegment) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-100.0}, LowerYBoundary{-100.0},
                             UpperXBoundary{100.0}, UpperYBoundary{100.0}};
  ScreenProjection projection{dim, bound};
  geometry::Segment s{geometry::Point{0, 20}, geometry::Point{20, 0}};
  ScreenLocationPair expected_pair{ScreenLocation{Right{100}, Down{80}},
                                   ScreenLocation{Right{120}, Down{100}}};
  const auto sp = projection.to_screen(s);

  EXPECT_THAT(sp, Optional(expected_pair));
}

TEST(TestScreenProjection, ProjectsPartiallyCoveredSegment) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-1.0}, LowerYBoundary{-1.0},
                             UpperXBoundary{1.0}, UpperYBoundary{1.0}};
  ScreenProjection projection{dim, bound};
  geometry::Segment s{geometry::Point{-1.0, 2.0}, geometry::Point{2.0, -1.0}};
  ScreenLocationPair expected_pair{ScreenLocation{Right{0}, Down{-100}},
                                   ScreenLocation{Right{300}, Down{200}}};
  const auto sp = projection.to_screen(s);

  EXPECT_THAT(sp, Optional(expected_pair));
}
TEST(TestScreenProjection, ProjectsPartiallyCoveredSegmentOnePointOnEdge) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-1.0}, LowerYBoundary{-1.0},
                             UpperXBoundary{1.0}, UpperYBoundary{1.0}};
  ScreenProjection projection{dim, bound};
  geometry::Segment s{geometry::Point{0.0, 1.0}, geometry::Point{2.0, -1.0}};
  ScreenLocationPair expected_pair{ScreenLocation{Right{100}, Down{0}},
                                   ScreenLocation{Right{300}, Down{200}}};
  const auto sp = projection.to_screen(s);

  EXPECT_THAT(sp, Optional(expected_pair));
}

// TEST(TestScreenProjection, InterpolatesPoint) {
//  ScreenDimensions dim{Right{200}, Down{200}};
//  CoordinateBoundaries bound{LowerXBoundary{-1.0}, LowerYBoundary{-1.0},
//                             UpperXBoundary{1.0}, UpperYBoundary{1.0}};
//  ScreenProjection projection{dim, bound};
//  ScreenLocation loc{Right{20}, Down{130}};
//  geometry::Point expected_point{-0.8, -0.3};
//  const auto p = projection.to_point(loc);
//
//  EXPECT_THAT(p, Near(expected_point, 1e-5));
//}

TEST(TestScreenProjection, TranslatesPoint) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-100.0}, LowerYBoundary{-100.0},
                             UpperXBoundary{100.0}, UpperYBoundary{100.0}};
  ScreenProjection projection{dim, bound};
  geometry::Point base{20, 30};
  ScreenLocationDifference difference{Right{40}, Down{50}};

  geometry::Point expected_point{60, -20};
  const auto p = projection.translate(base, difference);

  EXPECT_THAT(p, Near(expected_point, 1e-5));
}
TEST(TestScreenProjection, TranslatesPointNegativeDifference) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-100.0}, LowerYBoundary{-100.0},
                             UpperXBoundary{100.0}, UpperYBoundary{100.0}};
  ScreenProjection projection{dim, bound};
  geometry::Point base{20, 30};
  ScreenLocationDifference difference{Right{-40},
                                      Down{-50}};

  geometry::Point expected_point{-20, 80};
  const auto p = projection.translate(base, difference);

  EXPECT_THAT(p, Near(expected_point, 1e-5));
}
TEST(TestScreenProjection, TranslatesPointAtHalfRatio) {
  ScreenDimensions dim{Right{200}, Down{200}};
  CoordinateBoundaries bound{LowerXBoundary{-50.0}, LowerYBoundary{-50.0},
                             UpperXBoundary{50.0}, UpperYBoundary{50.0}};
  ScreenProjection projection{dim, bound};
  geometry::Point base{10, 15};
  ScreenLocationDifference difference{Right{40}, Down{50}};

  geometry::Point expected_point{30, -10};
  const auto p = projection.translate(base, difference);

  EXPECT_THAT(p, Near(expected_point, 1e-5));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
