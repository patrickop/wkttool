#pragma once
#include <wkttool/types.h>
#include <SFML/Graphics.hpp>
#include <boost/signals2.hpp>
#include <variant>
#include <cmath>

namespace wkttool {

std::optional<WindowEvent> to_internal(const sf::Event& event) {
  if (event.type == sf::Event::Closed) {
    return WindowClosed{};
  }
  return std::nullopt;
}

sf::Color to_sfml(const Color& color) {
  return sf::Color{color.red.get(), color.green.get(), color.blue.get()};
}

sf::RectangleShape to_sfml(const shape::Segment& segment) {
  // staticcast the uints, the result of subtraction may be negative
  const auto right_diff = static_cast<int>(segment.start.right.get()) - static_cast<int>(segment.end.right.get());
  const auto down_diff = static_cast<int>(segment.start.down.get()) - static_cast<int>(segment.end.down.get());
  const float length = std::hypot(right_diff, down_diff);
  const float thickness = segment.thickness.get();
  sf::RectangleShape result { sf::Vector2f {length, thickness}};
  result.setFillColor(to_sfml(segment.color));
  result.setPosition ( sf::Vector2f {segment.start.right.get(), segment.start.down.get()});
  return result; 
}
  


class SFMLWindowAdapter {
  public:
  SFMLWindowAdapter(const std::string& window_name,
                    const ScreenDimensions& dimensions) :
    
    window{sf::VideoMode(dimensions.right.get(), dimensions.down.get()), 
           window_name} {}

  void handle_events() {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (const auto& internal_event = to_internal(event); internal_event) {
        window_event_sig(*internal_event);
      }
    }
  }
  
  boost::signals2::connection connect(WindowEventCallback callback) {
    return window_event_sig.connect(callback); 
  }

  template <typename Object>
  void draw(const Object& object) {
    const auto s = to_sfml(object);
    window.draw(s);
  }

  void clear(const Color& color) {
    window.clear(to_sfml(color));
  }

  void display() {
    window.display();
  }

  private:
    sf::RenderWindow window;
    boost::signals2::signal<void(const WindowEvent&)> window_event_sig;
    
};


}
