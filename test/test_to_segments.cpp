#include <wkttool/to_segments.h>

#include "common.h"

using namespace wkttool;
using ::testing::UnorderedElementsAre;

TEST(TestToSegments, Polygon) {
  using namespace wkttool::geometry;

  const Polygon poly{
      {Point{0, 0}, Point{0, 100}, Point{100, 100}, Point{100, 0}, Point{0, 0}},
      {Point{10, 10}, Point{10, 90}, Point{90, 90}, Point{90, 10},
       Point{10, 10}}};

  const auto result = to_segments(poly);
  EXPECT_THAT(result, UnorderedElementsAre(
                          Near(Segment{Point{0, 0}, Point{0, 100}}, 1e-5),
                          Near(Segment{Point{0, 100}, Point{100, 100}}, 1e-5),
                          Near(Segment{Point{100, 100}, Point{100, 0}}, 1e-5),
                          Near(Segment{Point{100, 0}, Point{0, 0}}, 1e-5),
                          Near(Segment{Point{10, 10}, Point{10, 90}}, 1e-5),
                          Near(Segment{Point{10, 90}, Point{90, 90}}, 1e-5),
                          Near(Segment{Point{90, 90}, Point{90, 10}}, 1e-5),
                          Near(Segment{Point{90, 10}, Point{10, 10}}, 1e-5)));
}
TEST(TestToSegments, SubSample) {
  const size_t samples = 5;
  const double lower_x = 0;
  const double upper_x = 2.0 * M_PI;
  const auto color = black;
  const Thickness thickness{1};
  const auto segments = subsample([](const double e) { return std::sin(e); },
                                  lower_x, upper_x, samples);

  using namespace geometry;
  EXPECT_THAT(
      segments,
      UnorderedElementsAre(
          Near(Segment{Point{0, 0}, Point{M_PI / 2.0, 1}}, 1e-5),
          Near(Segment{Point{M_PI / 2.0, 1}, Point{M_PI, 0}}, 1e-5),
          Near(Segment{Point{M_PI, 0}, Point{M_PI * 1.5, -1}}, 1e-5),
          Near(Segment{Point{M_PI * 1.5, -1}, Point{M_PI * 2.0, 0}}, 1e-5)));
}
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
