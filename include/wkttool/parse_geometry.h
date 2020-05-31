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

  std::optional<geometry::Geometry> parse(const std::string& source) {
    try {
      if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source), "point")) {
        return read_as<geometry::Point>(source);
      }
      if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source), "linestring")) {
        return read_as<geometry::Linestring>(source);
      }
      if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source), "polygon")) {
        return read_as<geometry::Polygon>(source);
      }
      if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source), "multipoint")) {
        return read_as<geometry::MultiPoint>(source);
      }
      if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source), "multilinestring")) {
        return read_as<geometry::MultiLinestring>(source);
      }
      if (boost::algorithm::starts_with(boost::algorithm::to_lower_copy(source), "multipolygon")) {
        return read_as<geometry::MultiPolygon>(source);
      }
    } catch (const std::exception&) {
      return std::nullopt;
    }
    return std::nullopt;
  }
}