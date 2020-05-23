#include <gmock/gmock.h>
#include <wkttool/sfml_window_adapter.h>

using ::testing::MockFunction;
using ::testing::StrictMock;
using ::testing::FloatNear;
using ::testing::Optional;
using ::testing::VariantWith;
using namespace wkttool;
TEST (TestSFMLConversion, ConvertsColor)
{
  Color c {Red {234},
           Green {123},
           Blue {222} };
  const auto converted = to_sfml(c);
  EXPECT_EQ(converted.r, c.red.get());
  EXPECT_EQ(converted.g, c.green.get());
  EXPECT_EQ(converted.b, c.blue.get());
  EXPECT_EQ(converted.a, 255);
}
TEST (TestSFMLConversion, ConvertsSegment)
{
  shape::Segment s{ white, 
                    Thickness{ 5 },
                    ScreenLocation{ Right{ 300 }, Down{ 300 } },
                    ScreenLocation{ Right{ 500 }, Down{ 400 } } };
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
  //EXPECT_EQ(converted.r, c.red.get());
  //EXPECT_EQ(converted.g, c.green.get());
  //EXPECT_EQ(converted.b, c.blue.get());
  //EXPECT_EQ(converted.a, 255);
}
struct MockWindowEventListener {
  template <typename T>
  void operator()(const T& arg) {
    event = arg;
  }

  std::optional<WindowEventVariant> event = std::nullopt;
};
TEST (TestSFMLConversion, ConvertsWindowClosedEvent)
{
  MockWindowEventListener listener;

  sf::Event ev;
  ev.type = sf::Event::Closed;

  to_internal(ev, listener);
  WindowClosed expected_ev;
  ASSERT_TRUE(listener.event);
  ASSERT_TRUE(std::holds_alternative<WindowClosed>(*listener.event));
}
TEST (TestSFMLConversion, ConvertsMouseMovedEvent)
{
  MockWindowEventListener listener;

  sf::Event ev;
  ev.type = sf::Event::MouseMoved;
  ev.mouseMove.x = 22;
  ev.mouseMove.y = 33;

  to_internal(ev, listener);
  WindowClosed expected_ev;
  ASSERT_TRUE(listener.event);
  ASSERT_TRUE(std::holds_alternative<MouseMoved>(*listener.event));
  ASSERT_EQ(std::get<MouseMoved>(*listener.event).destination.right.get(), 22);
  ASSERT_EQ(std::get<MouseMoved>(*listener.event).destination.down.get(), 33);
}
int
main (int argc, char **argv)
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}
