#include <wkttool/make_grid.h>

#include <string>

#include "common.h"

using namespace wkttool;
using ::testing::_;
using ::testing::UnorderedElementsAre;

TEST(TestMakeGrid, BasicGrid) {
  CoordinateBoundaries bound{LowerXBoundary{-10}, LowerYBoundary{-5},
                             UpperXBoundary{10}, UpperYBoundary{5}};
  const auto grid = make_grid(bound, XStep{3}, YStep{2});

  using namespace wkttool::geometry;
  EXPECT_THAT(grid, UnorderedElementsAre(
                        Near(Segment{Point{-9, -5}, Point{-9, 5}}, 1e-5),
                        Near(Segment{Point{-6, -5}, Point{-6, 5}}, 1e-5),
                        Near(Segment{Point{-3, -5}, Point{-3, 5}}, 1e-5),
                        Near(Segment{Point{0, -5}, Point{0, 5}}, 1e-5),
                        Near(Segment{Point{3, -5}, Point{3, 5}}, 1e-5),
                        Near(Segment{Point{6, -5}, Point{6, 5}}, 1e-5),
                        Near(Segment{Point{9, -5}, Point{9, 5}}, 1e-5),
                        Near(Segment{Point{-10, -4}, Point{10, -4}}, 1e-5),
                        Near(Segment{Point{-10, -2}, Point{10, -2}}, 1e-5),
                        Near(Segment{Point{-10, 0}, Point{10, 0}}, 1e-5),
                        Near(Segment{Point{-10, 2}, Point{10, 2}}, 1e-5),
                        Near(Segment{Point{-10, 4}, Point{10, 4}}, 1e-5)));
}
TEST(TestMakeGrid, Axes) {
  CoordinateBoundaries bound{LowerXBoundary{-10}, LowerYBoundary{-5},
                             UpperXBoundary{10}, UpperYBoundary{5}};
  ScreenProjection proj{ScreenDimensions{Right{20}, Down{10}}, bound};
  const auto axes = make_axes(bound);

  using namespace wkttool::geometry;
  EXPECT_THAT(axes, UnorderedElementsAre(
                        Near(Segment{Point{-10, 0}, Point{10, 0}}, 1e-5),
                        Near(Segment{Point{0, -5}, Point{0, 5}}, 1e-5)));
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
