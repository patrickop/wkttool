#pragma once
#include <wkttool/types.h>
#include <wkttool/variant_signal.h>

#include <SFML/Graphics.hpp>
#include <boost/signals2.hpp>
#include <cmath>

#include "imgui-SFML.h"

namespace wkttool {
constexpr auto font_name = "/usr/share/fonts/TTF/arial.ttf";
template <typename Listener>
void to_internal(const sf::Event &event, Listener &listener) {
  if (event.type == sf::Event::Closed) {
    listener(WindowClosed{});
  }
  if (event.type == sf::Event::MouseMoved) {
    listener(MouseMoved{Right{event.mouseMove.x}, Down{event.mouseMove.y}});
  }
  if (event.type == sf::Event::MouseWheelScrolled) {
    listener(MouseWheelScrolled{
        ScreenLocation{Right{event.mouseWheelScroll.x},
                       Down{event.mouseWheelScroll.y}},
        MouseWheelScollAmount{event.mouseWheelScroll.delta}});
  }
  if (event.type == sf::Event::MouseButtonReleased) {
    // Todo: make nice
    if (event.mouseButton.button == sf::Mouse::Button::Right) {
      listener(MouseButtonUp{
          ScreenLocation{Right{event.mouseButton.x}, Down{event.mouseButton.y}},
          MouseButton::right});
    }
    if (event.mouseButton.button == sf::Mouse::Button::Left) {
      listener(MouseButtonUp{
          ScreenLocation{Right{event.mouseButton.x}, Down{event.mouseButton.y}},
          MouseButton::left});
    }
  }
  if (event.type == sf::Event::MouseButtonPressed) {
    // Todo: make nice
    if (event.mouseButton.button == sf::Mouse::Button::Right) {
      listener(MouseButtonDown{
          ScreenLocation{Right{event.mouseButton.x}, Down{event.mouseButton.y}},
          MouseButton::right});
    }
    if (event.mouseButton.button == sf::Mouse::Button::Left) {
      listener(MouseButtonDown{
          ScreenLocation{Right{event.mouseButton.x}, Down{event.mouseButton.y}},
          MouseButton::left});
    }
  }
}

sf::Color to_sfml(const Color &color) {
  return sf::Color{color.red.get(), color.green.get(), color.blue.get()};
}

sf::RectangleShape to_sfml(const drawable::Segment &segment) {
  const auto diff = segment.locations.second - segment.locations.first;
  const float length = std::hypot(diff.right.get(), diff.down.get());
  const float thickness = segment.thickness.get();
  sf::RectangleShape result{sf::Vector2f{length, thickness}};
  // rotation
  const float rot =
      std::atan2(diff.down.get(), diff.right.get()) * 180.0 / M_PI;
  result.setRotation(rot);
  result.setPosition(sf::Vector2f{
      static_cast<float>(std::get<0>(segment.locations).right.get()),
      static_cast<float>(std::get<0>(segment.locations).down.get())});
  result.setFillColor(to_sfml(segment.color));
  return result;
}

sf::Text to_sfml(const drawable::TextLabel &text) {
  static bool first = true;
  static sf::Font font;
  if (first) {
    if (!font.loadFromFile(font_name)) {
      std::cerr << "Error loading the font: " << font_name
                << ". Text will not display correctly" << std::endl;
    }
    first = false;
  }
  sf::Text result;
  result.setFont(font);
  result.setString(text.text);
  result.setPosition(
      sf::Vector2f{static_cast<float>(text.location.right.get()),
                   static_cast<float>(text.location.down.get())});
  result.setFillColor(to_sfml(text.color));
  result.setCharacterSize(text.size.get());
  return result;
}

class SFMLWindowAdapter {
 public:
  SFMLWindowAdapter(const std::string &window_name,
                    const ScreenDimensions &dimensions)
      :

        window{sf::VideoMode(dimensions.right.get(), dimensions.down.get()),
               window_name} {
    ImGui::SFML::Init(window);
  }
  ~SFMLWindowAdapter() { ImGui::SFML::Shutdown(); }

  void handle_events() {
    sf::Event event;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      to_internal(event, window_event_sig);
    }
    ImGui::SFML::Update(window, deltaClock.restart());
  }

  template <typename T>
  boost::signals2::connection connect(T callback) {
    return window_event_sig.connect(callback);
  }

  template <typename Object>
  void draw(const Object &object) {
    const auto s = to_sfml(object);
    window.draw(s);
  }

  template <typename Object>
  void draw(const std::vector<Object> &object) {
    for (const auto &c : object) {
      draw(c);
    }
  }

  void clear(const Color &color) { window.clear(to_sfml(color)); }

  void display() {
    ImGui::SFML::Render(window);
    window.display();
  }

 private:
  sf::RenderWindow window;
  sf::Clock deltaClock;
  VariantSignal<WindowEvent> window_event_sig;
};

}  // namespace wkttool
