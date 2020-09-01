#include <gmock/gmock.h>
#include <wkttool/label_position.h>

#include <string>

using ::testing::MockFunction;
using ::testing::StrictMock;
// using Signal = wkttool::VariantSignal<wkttool::TypeList<int, std::string> >;

using namespace wkttool;

TEST(TestLabelPosition, Point) {
  const geometry::Point input{1, 3};
  const auto result = label_position(input);
  ASSERT_NEAR(boost::geometry::get<0>(result), 1, 1e-5);
  ASSERT_NEAR(boost::geometry::get<1>(result), 3, 1e-5);
}
TEST(TestLabelPosition, Linestring) {
  const geometry::Linestring input{{6, 5}, {2, 3}};
  const auto result = label_position(input);
  ASSERT_NEAR(boost::geometry::get<0>(result), 6, 1e-5);
  ASSERT_NEAR(boost::geometry::get<1>(result), 5, 1e-5);
}
TEST(TestLabelPosition, Polygon) {
  const geometry::Polygon input{{{22, 33}, {12, 11}, {6, 5}, {2, 3}, {22, 33}}};
  const auto result = label_position(input);
  ASSERT_NEAR(boost::geometry::get<0>(result), 22, 1e-5);
  ASSERT_NEAR(boost::geometry::get<1>(result), 33, 1e-5);
}
TEST(TestLabelPosition, MultiPoint) {
  const geometry::MultiPoint input{{2, 3}, {1, 3}};
  const auto result = label_position(input);
  ASSERT_NEAR(boost::geometry::get<0>(result), 2, 1e-5);
  ASSERT_NEAR(boost::geometry::get<1>(result), 3, 1e-5);
}
TEST(TestLabelPosition, MultiLinestring) {
  const geometry::MultiLinestring input{{{6, 5}, {2, 3}}, {{44, 55}, {33, 11}}};
  const auto result = label_position(input);
  ASSERT_NEAR(boost::geometry::get<0>(result), 6, 1e-5);
  ASSERT_NEAR(boost::geometry::get<1>(result), 5, 1e-5);
}
TEST(TestLabelPosition, MultiPolygon) {
  const geometry::MultiPolygon input{{{{6, 5}, {2, 3}}},
                                     {{{44, 55}, {33, 11}}}};
  const auto result = label_position(input);
  ASSERT_NEAR(boost::geometry::get<0>(result), 6, 1e-5);
  ASSERT_NEAR(boost::geometry::get<1>(result), 5, 1e-5);
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
