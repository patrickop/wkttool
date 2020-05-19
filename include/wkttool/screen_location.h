#pragma once
#include <named_type.hpp>

namespace wkttool {
  using Down = fluent::NamedType<uint32_t, struct DownTag>;
  using Right = fluent::NamedType<uint32_t, struct RightTag>;

  struct screen_location {
    Down down;
    Right right;
  };
}
