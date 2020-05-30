#include <gmock/gmock.h>
#include <wkttool/to_drawables.h>

using namespace wkttool;
using ::testing::UnorderedElementsAre;
// TODO: add concept
class SimpleProjection {
 public:
  std::optional<ScreenLocationPair> to_screen(
      const geometry::Segment& segment) const {
    const auto x1 = static_cast<uint32_t>(x(segment.first));
    const auto y1 = static_cast<uint32_t>(y(segment.first));
    const auto x2 = static_cast<uint32_t>(x(segment.second));
    const auto y2 = static_cast<uint32_t>(y(segment.second));
    return ScreenLocationPair{ScreenLocation{Right{x1}, Down{y1}},
                              ScreenLocation{Right{x2}, Down{y2}}};
  }
};

TEST(TestDrawable, Segment) {}

TEST(TestDrawable, Polygon) {
  //  using namespace wkttool::geometry;
  //
  //  const Polygon poly{
  //      {Point{0, 0}, Point{0, 100}, Point{100, 100}, Point{100, 0}, Point{0,
  //      0}}, {Point{10, 10}, Point{10, 90}, Point{90, 90}, Point{90, 10},
  //       Point{10, 10}}};
  //
  //  SimpleProjection proj;
  //  const auto result = polygon_to_drawables(poly, proj, white, Thickness{2});
  //  EXPECT_THAT(result,
  //              UnorderedElementsAre(
  //                  drawable::Segment{white,
  //                                    Thickness{2},
  //                                    {ScreenLocation{Right{0}, Down{0}},
  //                                     ScreenLocation{Right{0}, Down{100}}}},
  //                  drawable::Segment{white,
  //                                    Thickness{2},
  //                                    {ScreenLocation{Right{0}, Down{100}},
  //                                     ScreenLocation{Right{100},
  //                                     Down{100}}}},
  //                  drawable::Segment{white,
  //                                    Thickness{2},
  //                                    {ScreenLocation{Right{100}, Down{100}},
  //                                     ScreenLocation{Right{100}, Down{0}}}},
  //                  drawable::Segment{white,
  //                                    Thickness{2},
  //                                    {ScreenLocation{Right{100}, Down{0}},
  //                                     ScreenLocation{Right{0}, Down{0}}}},
  //                  drawable::Segment{white,
  //                                    Thickness{2},
  //                                    {ScreenLocation{Right{10}, Down{10}},
  //                                     ScreenLocation{Right{10}, Down{90}}}},
  //                  drawable::Segment{white,
  //                                    Thickness{2},
  //                                    {ScreenLocation{Right{10}, Down{90}},
  //                                     ScreenLocation{Right{90}, Down{90}}}},
  //                  drawable::Segment{white,
  //                                    Thickness{2},
  //                                    {ScreenLocation{Right{90}, Down{90}},
  //                                     ScreenLocation{Right{90}, Down{10}}}},
  //                  drawable::Segment{white,
  //                                    Thickness{2},
  //                                    {ScreenLocation{Right{90}, Down{10}},
  //                                     ScreenLocation{Right{10},
  //                                     Down{10}}}}));
}
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
