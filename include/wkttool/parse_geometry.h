#pragma once
#include <wkttool/types.h>

#include <boost/algorithm/string.hpp>
#include <optional>

namespace wkttool {

template <typename T>
T read_as(const std::string& source) {
  T t;
  boost::geometry::read_wkt(source, t);
  return t;
}

std::optional<geometry::Geometry> parse_geometry(const std::string& source) {
  try {
    if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source),
                                      "point")) {
      return read_as<geometry::Point>(source);
    }
    if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source),
                                      "linestring")) {
      return read_as<geometry::Linestring>(source);
    }
    if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source),
                                      "polygon")) {
      return read_as<geometry::Polygon>(source);
    }
    if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source),
                                      "multipoint")) {
      return read_as<geometry::MultiPoint>(source);
    }
    if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source),
                                      "multilinestring")) {
      return read_as<geometry::MultiLinestring>(source);
    }
    if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source),
                                      "multipolygon")) {
      return read_as<geometry::MultiPolygon>(source);
    }
  } catch (const std::exception&) {
    return std::nullopt;
  }
  return std::nullopt;
}

using ParsedLine = std::pair<std::optional<std::string>, geometry::Geometry>;

std::optional<ParsedLine> parse(const std::string& source) {
  std::vector<std::string> result;
  boost::algorithm::split(result, source, boost::algorithm::is_any_of(":"));
  const auto geo = parse_geometry(result.size() == 1 ? result[0] : result[1]);
  std::optional<std::string> label = std::nullopt;
  if (result.size() > 1) {
    label = result[0];
  }
  if (geo) {
    return ParsedLine{label, *geo};
  } else {
    return std::nullopt;
  }
}

std::vector<std::string> tokenize(const std::string& raw) {
  std::vector<std::string> result;
  std::string working{raw};
  // remove linebreaks
  working.erase(remove(std::begin(working), std::end(working), '\n'),
                std::end(working));
  boost::algorithm::split(result, working, boost::algorithm::is_any_of(";"));
  // remove trailing and leading whitespace
  for (auto& l : result) {
    boost::algorithm::trim(l);
  }
  return result;
}
}  // namespace wkttool
