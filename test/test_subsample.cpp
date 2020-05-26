#include <gmock/gmock.h>
#include <wkttool/subsample.h>

#include <cmath>
#include <string>

namespace boost::geometry::model {
std::ostream &operator<<(std::ostream &os,
                         const wkttool::geometry::Segment &geo) {
  namespace bg = boost::geometry;
  return os << bg::wkt(geo);
}
}  // namespace boost::geometry::model

using namespace wkttool;
using ::testing::ElementsAre;
MATCHER_P2(Near, seg, tol, "") {
  namespace bg = boost::geometry;
  return bg::distance(seg.first, arg.first) < tol and
         bg::distance(seg.second, arg.second) < tol;
}
TEST(TestSubSample, TestSubSample) {
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
      ElementsAre(
          Near(Segment{Point{0, 0}, Point{M_PI / 2.0, 1}}, 1e-5),
          Near(Segment{Point{M_PI / 2.0, 1}, Point{M_PI, 0}}, 1e-5),
          Near(Segment{Point{M_PI, 0}, Point{M_PI * 1.5, -1}}, 1e-5),
          Near(Segment{Point{M_PI * 1.5, -1}, Point{M_PI * 2.0, 0}}, 1e-5)));
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
