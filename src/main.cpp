#include <wkttool/make_grid.h>
#include <wkttool/screen_projection.h>
#include <wkttool/sfml_window_adapter.h>
#include <wkttool/subsample.h>
#include <wkttool/to_segments.h>

void draw(wkttool::SFMLWindowAdapter &window,
          const wkttool::geometry::Point &center, const float scale,
          const wkttool::ScreenDimensions &dims) {
  using namespace wkttool;
  window.clear(white);
  CoordinateBoundaries bounds{LowerXBoundary{x(center) - 10.0 * scale},
                              LowerYBoundary{y(center) - 10.0 * scale},
                              UpperXBoundary{x(center) + 10.0 * scale},
                              UpperYBoundary{y(center) + 10.0 * scale}};
  ScreenProjection proj{dims, bounds};
  const auto grid = make_grid(bounds, XStep{2.0}, YStep{2.0});
  const auto axes = make_axes(bounds);
  const auto samples =
      subsample([](const double &x) { return std::sin(x); },
                bounds.lower_x.get(), bounds.upper_x.get(), 10000);
  geometry::Polygon poly{
      {geometry::Point{1, 1}, geometry::Point{1, 4}, geometry::Point{4, 5},
       geometry::Point{4, 1}, geometry::Point{1, 1}},
      {geometry::Point{2, 2}, geometry::Point{2, 3}, geometry::Point{3, 3},
       geometry::Point{3, 2}, geometry::Point{2, 2}}};
  const auto poly_segments = to_segments(poly);
  const geometry::Linestring ls{geometry::Point{0, 0}, geometry::Point{33, 3},
                                geometry::Point{44, 55}};
  const auto ls_segments = to_segments(ls);
  const geometry::Point pt{1, 1};
  window.draw(segments_to_drawables(grid, proj, grey, Thickness{1}));
  window.draw(segments_to_drawables(axes, proj, black, Thickness{2}));
  window.draw(segments_to_drawables(samples, proj, black, Thickness{3}));
  window.draw(segments_to_drawables(poly_segments, proj, black, Thickness{3}));
  window.draw(segments_to_drawables(ls_segments, proj, red, Thickness{5}));
  window.draw(
      point_to_drawables(pt, proj, black, Thickness{1}, Right{10}, Down{10}));

  window.display();
}
int main(int, char **) {
  using namespace wkttool;
  const ScreenDimensions dims{Right{800}, Down{600}};

  SFMLWindowAdapter window{"my window",
                           ScreenDimensions{Right{800}, Down{600}}};

  bool running = true;
  float scale = 1.0;
  bool redraw = true;
  std::optional<ScreenLocation> mouse_pressed_down_at = std::nullopt;
  geometry::Point center{0, 0};
  window.connect([&running](const WindowClosed &) { running = false; });
  window.connect([&scale, &redraw](const MouseWheelScrolled &ev) {
    scale = scale * std::pow(1.20, -ev.amount.get());
    redraw = true;
  });
  window.connect([&mouse_pressed_down_at](const MouseButtonDown &ev) {
    mouse_pressed_down_at = ev.location;
  });
  window.connect([&mouse_pressed_down_at](const MouseButtonUp &ev) {
    mouse_pressed_down_at = std::nullopt;
  });
  window.connect([&mouse_pressed_down_at, &redraw, &center, &scale,
                  &dims](const MouseMoved &ev) {
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
      redraw = true;
    }
  });
  auto last_frame = std::chrono::system_clock::now();
  while (running) {
    window.handle_events();
    if (redraw) {
      draw(window, center, scale, dims);
      // redraw = false;
    }
    const auto new_frame = std::chrono::system_clock::now();
    std::cout << "Frame time: " << (new_frame - last_frame).count() / 1e6
              << std::endl;

    last_frame = new_frame;
  }

  return EXIT_SUCCESS;
}
