#include <wkttool/make_grid.h>
#include <wkttool/screen_projection.h>
#include <wkttool/sfml_window_adapter.h>

int main(int, char **) {
  using namespace wkttool;
  const ScreenDimensions dims{Right{800}, Down{600}};

  SFMLWindowAdapter window{"my window",
                           ScreenDimensions{Right{800}, Down{600}}};

  bool running = true;
  float scale = 1.0;
  std::optional<ScreenLocation> mouse_pressed_down_at = std::nullopt;
  geometry::Point center{0, 0};
  window.connect([&running](const WindowClosed &) { running = false; });
  window.connect([&scale](const MouseWheelScrolled &ev) {
    scale = scale * std::pow(1.20, -ev.amount.get());
  });
  window.connect([&mouse_pressed_down_at](const MouseButtonDown &ev) {
    mouse_pressed_down_at = ev.location;
  });
  window.connect([&mouse_pressed_down_at](const MouseButtonUp &ev) {
    mouse_pressed_down_at = std::nullopt;
  });
  window.connect(
      [&mouse_pressed_down_at, &center, &scale, &dims](const MouseMoved &ev) {
        if (mouse_pressed_down_at) {
          const auto shift = *mouse_pressed_down_at - ev.destination;
          CoordinateBoundaries bounds{LowerXBoundary{x(center) - 10.0 * scale},
                                      LowerYBoundary{y(center) - 10.0 * scale},
                                      UpperXBoundary{x(center) + 10.0 * scale},
                                      UpperYBoundary{y(center) + 10.0 * scale}};
          ScreenProjection proj{dims, bounds};
          center = proj.translate(center, shift);
          std::cout << "Shift by " << shift << std::endl;
          std::cout << "Center moved to " << boost::geometry::wkt(center)
                    << std::endl;
          mouse_pressed_down_at = ev.destination;
        }
      });

  while (running) {
    window.handle_events();
    window.clear(white);

    CoordinateBoundaries bounds{LowerXBoundary{x(center) - 10.0 * scale},
                                LowerYBoundary{y(center) - 10.0 * scale},
                                UpperXBoundary{x(center) + 10.0 * scale},
                                UpperYBoundary{y(center) + 10.0 * scale}};
    ScreenProjection proj{dims, bounds};
    window.draw(make_grid(bounds, proj, XStep{2.0}, YStep{2.0}, black, grey));

    window.display();
  }

  return EXIT_SUCCESS;
}
