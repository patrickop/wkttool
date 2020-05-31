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

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
