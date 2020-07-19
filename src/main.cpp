#include <wkttool/make_grid.h>
#include <wkttool/screen_projection.h>
#include <wkttool/sfml_window_adapter.h>
#include <wkttool/subsample.h>
#include <wkttool/to_segments.h>
#include <wkttool/parse_geometry.h>
#include <imgui.h>
#include <imgui_stdlib.h>
constexpr auto initial_scenario = "POINT(1 2);\n\
LINESTRING(0 0,2 2,3 1);\n\
POLYGON((0 0,0 7,4 2,2 0,0 0));\n\
MULTIPOINT(1.5 2.5, -3 -2);\n\
MULTILINESTRING((-1 2, -2 1),(7 7, 8 10));\n\
MULTIPOLYGON(((0 0,0 -7,-4 -2,-2 0,0 0)), ((5 -2, 6 -3, 4 -2.5, 5 -2)))";
template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;
std::string draw(wkttool::SFMLWindowAdapter &window,
          const wkttool::geometry::Point &center, const float scale,
          const wkttool::ScreenDimensions &dims, const std::string& shapes) {
  using namespace wkttool;
  std::stringstream errors;
  window.clear(white);
  CoordinateBoundaries bounds{LowerXBoundary{x(center) - 10.0 * scale},
                              LowerYBoundary{y(center) - 10.0 * scale},
                              UpperXBoundary{x(center) + 10.0 * scale},
                              UpperYBoundary{y(center) + 10.0 * scale}};
  ScreenProjection proj{dims, bounds};
  const auto grid = make_grid(bounds, XStep{2.0}, YStep{2.0});
  const auto axes = make_axes(bounds);
  window.draw(segments_to_drawables(grid, proj, grey, Thickness{1}));
  window.draw(segments_to_drawables(axes, proj, black, Thickness{2}));
  const auto tokens = tokenize(shapes);
  for (const auto& token : tokens) {
    try {
      const auto geo = parse(token);
      if (geo) {
        std::visit(overload{
          [&proj, &window] (const geometry::Polygon& poly) {
              const auto segments =  to_segments(poly);
              window.draw(segments_to_drawables(segments, proj, black, Thickness{3}));},
          [&proj, &window] (const geometry::MultiPolygon& polys) {
              const auto segments =  to_segments(polys);
              window.draw(segments_to_drawables(segments, proj, black, Thickness{3}));},
          [&proj, &window] (const geometry::MultiLinestring& lss) {
              const auto segments =  to_segments(lss);
              window.draw(segments_to_drawables(segments, proj, black, Thickness{3}));},
          [&proj, &window] (const geometry::Linestring& ls) {
              const auto segments =  to_segments(ls);
              window.draw(segments_to_drawables(segments, proj, black, Thickness{3}));},
          [&proj, &window] (const geometry::Point& point) {
              window.draw(point_to_drawables(
              point, proj, black, Thickness{1}, Right{10}, Down{10}));},
          [&proj, &window] (const geometry::MultiPoint& points) {
              window.draw(points_to_drawables(
              points, proj, black, Thickness{1}, Right{10}, Down{10}));},
          [&errors, &token] (const auto& geo) {
              errors << "Not implemented " << token << std::endl; }
            }, *geo);
      } else {
        errors << "unidentified object type " << token << std::endl;
      }
    } catch (const std::exception& ex) {
      errors << ex.what() << std::endl;
    }
  }
  return errors.str();

}
int main(int, char **) {
  using namespace wkttool;
  const ScreenDimensions dims{Right{800}, Down{600}};

  SFMLWindowAdapter window{"my window",
                           ScreenDimensions{Right{800}, Down{600}}};

  bool running = true;
  float scale = 1.0;
  bool redraw = true;
  bool is_window_hovered = false;
  std::optional<ScreenLocation> mouse_pressed_down_at = std::nullopt;
  geometry::Point center{0, 0};
  window.connect([&running](const WindowClosed &) { running = false; });
  window.connect([&scale, &redraw, &is_window_hovered](const MouseWheelScrolled &ev) {
    if (is_window_hovered) return;
    scale = scale * std::pow(1.20, -ev.amount.get());
    redraw = true;
  });
  window.connect([&mouse_pressed_down_at, &is_window_hovered](const MouseButtonDown &ev) {
    mouse_pressed_down_at = ev.location;
  });
  window.connect([&mouse_pressed_down_at, &is_window_hovered](const MouseButtonUp &ev) {
    mouse_pressed_down_at = std::nullopt;
  });
  window.connect([&mouse_pressed_down_at, &redraw, &center, &scale,
                  &dims, &is_window_hovered](const MouseMoved &ev) {
    if (is_window_hovered) return;
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
  std::string bla {initial_scenario};
  std::string errors;
  while (running) {
    window.handle_events();
    if (redraw) {
      errors = draw(window, center, scale, dims, bla);
    }
    const auto new_frame = std::chrono::system_clock::now();
    std::stringstream framestring;
    framestring << "Frame Time: " << (new_frame - last_frame).count() / 1e6;

    last_frame = new_frame;
    ImGui::Begin("Control");
    ImGui::GetClipboardText();
    ImGui::Text(framestring.str().c_str());
    ImGui::Text("Shapes:");
    ImGui::InputTextMultiline("##Shapes", &bla);
    is_window_hovered = ImGui::IsWindowHovered() or ImGui::IsWindowFocused();
    ImGui::Text("Errors:");
    ImGui::Text(errors.c_str());
    ImGui::End();
    window.display();
  }

  return EXIT_SUCCESS;
}
