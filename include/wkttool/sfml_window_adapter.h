#pragma once
#include <wkttool/types.h>
#include <SFML/Graphics.hpp>
#include <boost/signals2.hpp>
#include <variant>

namespace wkttool {

struct WindowClosed {};
using WindowEvent = std::variant<WindowClosed>;
using WindowEventCallback = std::function<void(const WindowEvent&)>;

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
      if (event.type == sf::Event::Closed) {
        window_event_sig(WindowClosed{});
      }
    }
  }
  
  boost::signals2::connection connect(WindowEventCallback callback) {
    return window_event_sig.connect(callback); 
  }

  void draw(const shape::Segment& segment) {
    sf::Vertex seg[] =
    {
      sf::Vertex(sf::Vector2f(segment.first.right.get(), 
                              segment.first.down.get())),
      sf::Vertex(sf::Vector2f(segment.second.right.get(), 
                              segment.second.down.get())),
    };

    window.draw(seg, 2, sf::Lines);
  }

  void clear() {
    window.clear(sf::Color::Black);
  }

  void display() {
    window.display();
  }

  private:
    sf::RenderWindow window;
    boost::signals2::signal<void(const WindowEvent&)> window_event_sig;
    
};


}
