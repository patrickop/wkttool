#pragma once
#include <SFML/Graphics.hpp>
#include <boost/signals2.hpp>
#include <cmath>
#include <wkttool/types.h>
#include <wkttool/variant_signal.h>

namespace wkttool
{

template <typename Listener>
void
to_internal (const sf::Event &event, Listener& listener)
{
  if (event.type == sf::Event::Closed)
    {
      listener (WindowClosed{});
    }
  if (event.type == sf::Event::MouseMoved)
    {
      listener (
          MouseMoved{ Right{ static_cast<uint32_t>(event.mouseMove.x) }, Down{ static_cast<uint32_t>(event.mouseMove.y) } });
    }
}

sf::Color
to_sfml (const Color &color)
{
  return sf::Color{ color.red.get (), color.green.get (), color.blue.get () };
}

sf::RectangleShape
to_sfml (const shape::Segment &segment)
{
  // staticcast the uints, the result of subtraction may be negative
  const auto right_diff = static_cast<int> (segment.start.right.get ())
                          - static_cast<int> (segment.end.right.get ());
  const auto down_diff = static_cast<int> (segment.start.down.get ())
                         - static_cast<int> (segment.end.down.get ());
  const float length = std::hypot (right_diff, down_diff);
  const float thickness = segment.thickness.get ();
  sf::RectangleShape result{ sf::Vector2f{ length, thickness } };
  // rotation
  const float rot
      = std::atan2 (static_cast<float> (segment.end.down.get ()
                                        - segment.start.down.get ()),
                    static_cast<float> (segment.end.right.get ()
                                        - segment.start.right.get ()))
        * 180 / M_PI;
  result.setRotation (rot);
  result.setPosition (
      sf::Vector2f{ static_cast<float> (segment.start.right.get ()),
                    static_cast<float> (segment.start.down.get ()) });
  result.setFillColor (to_sfml (segment.color));
  return result;
}

class SFMLWindowAdapter
{
public:
  SFMLWindowAdapter (const std::string &window_name,
                     const ScreenDimensions &dimensions)
      :

        window{ sf::VideoMode (dimensions.right.get (),
                               dimensions.down.get ()),
                window_name }
  {
  }

  void
  handle_events ()
  {
    sf::Event event;
    while (window.pollEvent (event))
      {
        to_internal (event, window_event_sig);
      }
  }

  template <typename T>
  boost::signals2::connection
  connect (T callback)
  {
    return window_event_sig.connect (callback);
  }

  template <typename Object>
  void
  draw (const Object &object)
  {
    const auto s = to_sfml (object);
    window.draw (s);
  }

  void
  clear (const Color &color)
  {
    window.clear (to_sfml (color));
  }

  void
  display ()
  {
    window.display ();
  }

private:
  sf::RenderWindow window;
  VariantSignal<WindowEvent> window_event_sig;
};

}
