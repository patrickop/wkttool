#include <wkttool/parse_geometry.h>

#include "common.h"

using namespace wkttool;
using ::testing::_;
using ::testing::UnorderedElementsAre;
using ::testing::Optional;
using ::testing::VariantWith;
using ::testing::Types;
using ::testing::Pair;
using ::testing::Eq;
template <typename T>
class TestParseGeometry : public ::testing::Test {
};
using ParsedTypes = Types<
  geometry::Point,
  geometry::Linestring,
  geometry::Polygon,
  geometry::MultiPoint,
  geometry::MultiLinestring,
  geometry::MultiPolygon>;
TYPED_TEST_SUITE(TestParseGeometry, ParsedTypes);

// We trust boost geometry to parse the values correctly
TYPED_TEST(TestParseGeometry, PicksCorrectType) {
  const TypeParam original{};
  std::stringstream wkt_rep;
  wkt_rep << boost::geometry::wkt(original);
  const auto result = parse_geometry(wkt_rep.str());
  EXPECT_THAT(result, Optional(VariantWith<TypeParam>(_)));
}
TEST(TestTokenizer, TestSplits){
  const std::string raw = "A;B";
  EXPECT_THAT(tokenize(raw),
      UnorderedElementsAre(
        "A",
        "B"));

}
TEST(TestTokenizer, TestRemovesNewlines){
  const std::string raw = "A;\nB\n1";
  EXPECT_THAT(tokenize(raw),
      UnorderedElementsAre(
        "A",
        "B1"));

}
TEST(TestTokenizer, TestRemovesTrailingLeadingWhitespace){
  const std::string raw = "  A  ; B 1  ";
  EXPECT_THAT(tokenize(raw),
      UnorderedElementsAre(
        "A",
        "B 1"));

}
TEST(TestParse, TestExtractsLabel) {
  EXPECT_THAT(parse("mylabel:POINT()"),
      Optional(
        Pair(
          Optional(std::string{"mylabel"}),
          VariantWith<geometry::Point>(_))));
}

TEST(TestParse, TestNoLabel) {
  EXPECT_THAT(parse("POINT()"),
      Optional(
        Pair(
          std::nullopt,
          VariantWith<geometry::Point>(_))));
}
TEST(TestParse, TestNoValidGeoNoLabel) {
  EXPECT_THAT(parse("ODDTHING()"), Eq(std::nullopt));
}
TEST(TestParse, TestNoValidGeo) {
  EXPECT_THAT(parse("mylabel:ODDTHING()"), Eq(std::nullopt));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
