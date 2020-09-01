#include <gmock/gmock.h>
#include <wkttool/to_drawables.h>

using namespace wkttool;
using ::testing::UnorderedElementsAre;
// TODO: add concept
class SimpleProjection {
 public:
  ScreenLocationPair to_screen(const geometry::Segment& segment) const {
    const auto x1 = static_cast<int>(x(segment.first));
    const auto y1 = static_cast<int>(y(segment.first));
    const auto x2 = static_cast<int>(x(segment.second));
    const auto y2 = static_cast<int>(y(segment.second));
    return ScreenLocationPair{ScreenLocation{Right{x1}, Down{y1}},
                              ScreenLocation{Right{x2}, Down{y2}}};
  }
  ScreenLocation to_screen(const geometry::Point& point) const {
    const auto x1 = static_cast<int>(x(point));
    const auto y1 = static_cast<int>(y(point));
    return ScreenLocation{Right{x1}, Down{y1}};
  }
  geometry::Point translate(const geometry::Point& base,
                            const ScreenLocationDifference& difference) const {
    const double x_val = x(base) + static_cast<double>(difference.right.get());
    const double y_val = y(base) + static_cast<double>(difference.down.get());
    return geometry::Point{x_val, y_val};
  }
};

TEST(TestDrawable, Segments) {
  using namespace geometry;
  std::vector<Segment> geos{Segment{Point{2, 3}, Point{3, 3}},
                            Segment{Point{5, 5}, Point{6, 6}}};
  SimpleProjection proj;
  const auto result = segments_to_drawables(geos, proj, white, Thickness{2});
  EXPECT_THAT(result,
              UnorderedElementsAre(
                  drawable::Segment{white,
                                    Thickness{2},
                                    {ScreenLocation{Right{2}, Down{3}},
                                     ScreenLocation{Right{3}, Down{3}}}},
                  drawable::Segment{white,
                                    Thickness{2},
                                    {ScreenLocation{Right{5}, Down{5}},
                                     ScreenLocation{Right{6}, Down{6}}}}));
}

TEST(TestDrawable, Point) {
  using namespace geometry;
  Point pt{2, 3};
  SimpleProjection proj;
  const auto result =
      point_to_drawables(pt, proj, white, Thickness{2}, Right{2}, Down{1});
  EXPECT_THAT(result,
              UnorderedElementsAre(
                  drawable::Segment{white,
                                    Thickness{2},
                                    {ScreenLocation{Right{0}, Down{2}},
                                     ScreenLocation{Right{4}, Down{4}}}},
                  drawable::Segment{white,
                                    Thickness{2},
                                    {ScreenLocation{Right{0}, Down{4}},
                                     ScreenLocation{Right{4}, Down{2}}}}));
}

TEST(TestDrawable, MultiPoint) {
  using namespace geometry;
  MultiPoint pts{{2, 3}, {12, 13}};
  SimpleProjection proj;
  const auto result =
      points_to_drawables(pts, proj, white, Thickness{2}, Right{2}, Down{1});
  EXPECT_THAT(result,
              UnorderedElementsAre(
                  drawable::Segment{white,
                                    Thickness{2},
                                    {ScreenLocation{Right{0}, Down{2}},
                                     ScreenLocation{Right{4}, Down{4}}}},
                  drawable::Segment{white,
                                    Thickness{2},
                                    {ScreenLocation{Right{0}, Down{4}},
                                     ScreenLocation{Right{4}, Down{2}}}},
                  drawable::Segment{white,
                                    Thickness{2},
                                    {ScreenLocation{Right{10}, Down{12}},
                                     ScreenLocation{Right{14}, Down{14}}}},
                  drawable::Segment{white,
                                    Thickness{2},
                                    {ScreenLocation{Right{10}, Down{14}},
                                     ScreenLocation{Right{14}, Down{12}}}}));
}

TEST(TestDrawable, Text) {
  using namespace geometry;
  const auto in = "Hello";
  SimpleProjection proj;
  const auto result =
      text_to_drawable(in, geometry::Point{1, 2}, proj, white, PointSize{22});
  const ScreenLocation expected_loc{Right{1}, Down{2}};
  const PointSize expected_size{22};
  EXPECT_EQ(result.color, white);
  EXPECT_EQ(result.location, expected_loc);
  EXPECT_EQ(result.text, "Hello");
  // TODO
  // EXPECT_EQ(result.size, expected_size);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
