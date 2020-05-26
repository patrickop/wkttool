#include <gmock/gmock.h>
#include <wkttool/make_grid.h>

#include <string>

using namespace wkttool;
using ::testing::_;
using ::testing::UnorderedElementsAre;

TEST(TestMakeGrid, BasicGrid) {
  CoordinateBoundaries bound{LowerXBoundary{-10}, LowerYBoundary{-5},
                             UpperXBoundary{10}, UpperYBoundary{5}};
  ScreenProjection proj{ScreenDimensions{Right{20}, Down{10}}, bound};
  const auto grid = make_grid(bound, proj, XStep{3}, YStep{2}, grey);

  EXPECT_THAT(grid, UnorderedElementsAre(
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{0}, Down{1}},
                                        ScreenLocation{Right{19}, Down{1}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{0}, Down{3}},
                                        ScreenLocation{Right{19}, Down{3}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{0}, Down{5}},
                                        ScreenLocation{Right{19}, Down{5}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{0}, Down{7}},
                                        ScreenLocation{Right{19}, Down{7}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{0}, Down{9}},
                                        ScreenLocation{Right{19}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{1}, Down{0}},
                                        ScreenLocation{Right{1}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{4}, Down{0}},
                                        ScreenLocation{Right{4}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{7}, Down{0}},
                                        ScreenLocation{Right{7}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{10}, Down{0}},
                                        ScreenLocation{Right{10}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{13}, Down{0}},
                                        ScreenLocation{Right{13}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{16}, Down{0}},
                                        ScreenLocation{Right{16}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{19}, Down{0}},
                                        ScreenLocation{Right{19}, Down{9}}}}));
}
TEST(TestMakeGrid, NoLinesJustOffScreen) {
  CoordinateBoundaries bound{LowerXBoundary{-10}, LowerYBoundary{-5},
                             UpperXBoundary{10}, UpperYBoundary{5}};
  ScreenProjection proj{ScreenDimensions{Right{20}, Down{10}}, bound};
  const auto grid = make_grid(bound, proj, XStep{5}, YStep{5}, grey);

  EXPECT_THAT(grid, UnorderedElementsAre(
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{0}, Down{0}},
                                        ScreenLocation{Right{19}, Down{0}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{0}, Down{5}},
                                        ScreenLocation{Right{19}, Down{5}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{0}, Down{0}},
                                        ScreenLocation{Right{0}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{5}, Down{0}},
                                        ScreenLocation{Right{5}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{10}, Down{0}},
                                        ScreenLocation{Right{10}, Down{9}}}},
                        shape::Segment{grey,
                                       Thickness{1},
                                       {ScreenLocation{Right{15}, Down{0}},
                                        ScreenLocation{Right{15}, Down{9}}}}));
}
TEST(TestMakeGrid, Axes) {
  CoordinateBoundaries bound{LowerXBoundary{-10}, LowerYBoundary{-5},
                             UpperXBoundary{10}, UpperYBoundary{5}};
  ScreenProjection proj{ScreenDimensions{Right{20}, Down{10}}, bound};
  const auto axes = make_axes(bound, proj, black);

  EXPECT_THAT(axes, UnorderedElementsAre(
                        shape::Segment{black,
                                       Thickness{1},
                                       {ScreenLocation{Right{0}, Down{5}},
                                        ScreenLocation{Right{19}, Down{5}}}},
                        shape::Segment{black,
                                       Thickness{1},
                                       {ScreenLocation{Right{10}, Down{0}},
                                        ScreenLocation{Right{10}, Down{9}}}}));
}
TEST(TestMakeGrid, AxesOffScreen) {
  CoordinateBoundaries bound{LowerXBoundary{5}, LowerYBoundary{1},
                             UpperXBoundary{10}, UpperYBoundary{5}};
  ScreenProjection proj{ScreenDimensions{Right{20}, Down{10}}, bound};
  const auto axes = make_axes(bound, proj, black);

  EXPECT_TRUE(axes.empty());
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
