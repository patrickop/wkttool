#include <gmock/gmock.h>
#include <wkttool/sfml_window_adapter.h>

using ::testing::FloatNear;
using ::testing::MockFunction;
using ::testing::Optional;
using ::testing::StrictMock;
using ::testing::VariantWith;
using namespace wkttool;
TEST(TestSFMLConversion, ConvertsColor) {
  Color c{Red{234}, Green{123}, Blue{222}};
  const auto converted = to_sfml(c);
  EXPECT_EQ(converted.r, c.red.get());
  EXPECT_EQ(converted.g, c.green.get());
  EXPECT_EQ(converted.b, c.blue.get());
  EXPECT_EQ(converted.a, 255);
}
TEST(TestSFMLConversion, ConvertsSegment) {
  drawable::Segment s{white,
                      Thickness{5},
                      {ScreenLocation{Right{300}, Down{300}},
                       ScreenLocation{Right{500}, Down{400}}}};
  const auto converted = to_sfml(s);
  EXPECT_THAT(converted.getSize().x, FloatNear(224, 0.5));
  EXPECT_THAT(converted.getSize().y, FloatNear(5, 0.5));
  EXPECT_THAT(converted.getPosition().x, FloatNear(300, 0.5));
  EXPECT_THAT(converted.getPosition().y, FloatNear(300, 0.5));
  EXPECT_EQ(converted.getFillColor().r, 255);
  EXPECT_EQ(converted.getFillColor().g, 255);
  EXPECT_EQ(converted.getFillColor().b, 255);
  EXPECT_EQ(converted.getFillColor().a, 255);
  EXPECT_THAT(converted.getRotation(), FloatNear(27.0, 0.5));
}
TEST(TestSFMLConversion, ConvertsSmallUpwardSegment) {
  drawable::Segment s{white,
                      Thickness{1},
                      {ScreenLocation{Right{300}, Down{300}},
                       ScreenLocation{Right{302}, Down{298}}}};
  const auto converted = to_sfml(s);
  EXPECT_THAT(converted.getSize().x, FloatNear(3, 0.5));
  EXPECT_THAT(converted.getSize().y, FloatNear(1, 0.5));
  EXPECT_THAT(converted.getPosition().x, FloatNear(300, 0.5));
  EXPECT_THAT(converted.getPosition().y, FloatNear(300, 0.5));
  EXPECT_EQ(converted.getFillColor().r, 255);
  EXPECT_EQ(converted.getFillColor().g, 255);
  EXPECT_EQ(converted.getFillColor().b, 255);
  EXPECT_EQ(converted.getFillColor().a, 255);
  EXPECT_THAT(converted.getRotation(), FloatNear(315.0, 0.5));
}
struct MockWindowEventListener {
  template <typename T>
  void operator()(const T& arg) {
    event = arg;
  }

  std::optional<WindowEventVariant> event = std::nullopt;
};
TEST(TestSFMLConversion, ConvertsWindowClosedEvent) {
  MockWindowEventListener listener;

  sf::Event ev;
  ev.type = sf::Event::Closed;

  to_internal(ev, listener);
  WindowClosed expected_ev;
  ASSERT_TRUE(listener.event);
  ASSERT_TRUE(std::holds_alternative<WindowClosed>(*listener.event));
}
TEST(TestSFMLConversion, ConvertsMouseMovedEvent) {
  MockWindowEventListener listener;

  sf::Event ev;
  ev.type = sf::Event::MouseMoved;
  ev.mouseMove.x = 22;
  ev.mouseMove.y = 33;

  to_internal(ev, listener);
  WindowClosed expected_ev;
  ASSERT_TRUE(listener.event);
  // todo: use variant matcher
  ASSERT_TRUE(std::holds_alternative<MouseMoved>(*listener.event));
  ASSERT_EQ(std::get<MouseMoved>(*listener.event).destination.right.get(), 22);
  ASSERT_EQ(std::get<MouseMoved>(*listener.event).destination.down.get(), 33);
}
TEST(TestSFMLConversion, ConvertsMouseButtonUpEvent) {
  MockWindowEventListener listener;

  sf::Event ev;
  ev.type = sf::Event::MouseButtonReleased;
  ev.mouseButton.x = 22;
  ev.mouseButton.y = 33;
  ev.mouseButton.button = sf::Mouse::Button::Left;

  to_internal(ev, listener);
  WindowClosed expected_ev;
  ASSERT_TRUE(listener.event);
  // todo: use variant matcher
  ASSERT_TRUE(std::holds_alternative<MouseButtonUp>(*listener.event));
  ASSERT_EQ(std::get<MouseButtonUp>(*listener.event).location.right.get(), 22);
  ASSERT_EQ(std::get<MouseButtonUp>(*listener.event).location.down.get(), 33);
  ASSERT_EQ(std::get<MouseButtonUp>(*listener.event).button, MouseButton::left);
}
TEST(TestSFMLConversion, ConvertsMouseButtonDownEvent) {
  MockWindowEventListener listener;

  sf::Event ev;
  ev.type = sf::Event::MouseButtonPressed;
  ev.mouseButton.x = 22;
  ev.mouseButton.y = 33;
  ev.mouseButton.button = sf::Mouse::Button::Right;

  to_internal(ev, listener);
  WindowClosed expected_ev;
  ASSERT_TRUE(listener.event);
  // todo: use variant matcher
  ASSERT_TRUE(std::holds_alternative<MouseButtonDown>(*listener.event));
  ASSERT_EQ(std::get<MouseButtonDown>(*listener.event).location.right.get(),
            22);
  ASSERT_EQ(std::get<MouseButtonDown>(*listener.event).location.down.get(), 33);
  ASSERT_EQ(std::get<MouseButtonDown>(*listener.event).button,
            MouseButton::right);
}
TEST(TestSFMLConversion, ConvertsMouseWheelScrolledEvent) {
  MockWindowEventListener listener;

  sf::Event ev;
  ev.type = sf::Event::MouseWheelScrolled;
  ev.mouseWheelScroll.x = 22;
  ev.mouseWheelScroll.y = 33;
  ev.mouseWheelScroll.delta = 1.6;

  to_internal(ev, listener);
  WindowClosed expected_ev;
  ASSERT_TRUE(listener.event);
  // todo: use variant matcher
  ASSERT_TRUE(std::holds_alternative<MouseWheelScrolled>(*listener.event));
  ASSERT_EQ(std::get<MouseWheelScrolled>(*listener.event).position.right.get(),
            22);
  ASSERT_EQ(std::get<MouseWheelScrolled>(*listener.event).position.down.get(),
            33);
  ASSERT_NEAR(std::get<MouseWheelScrolled>(*listener.event).amount.get(), 1.6,
              1e-5);
}
TEST(TestSFMLConversion, ConvertsTextLabel) {
  drawable::TextLabel label{white, ScreenLocation{Right{1}, Down{2}}, "Hello",
                            PointSize{33}};
  const auto result = to_sfml(label);
  ASSERT_EQ(result.getString(), "Hello");
  ASSERT_NEAR(result.getPosition().x, 1, 1e-5);
  ASSERT_NEAR(result.getPosition().y, 2, 1e-5);
  ASSERT_NEAR(result.getCharacterSize(), 33, 1e-5);
  EXPECT_EQ(result.getFillColor().r, 255);
  EXPECT_EQ(result.getFillColor().g, 255);
  EXPECT_EQ(result.getFillColor().b, 255);
  EXPECT_EQ(result.getFillColor().a, 255);
}
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
