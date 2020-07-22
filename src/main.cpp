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

struct Shape {
  std::string label;
  wkttool::geometry::Geometry geometry;
  wkttool::Thickness thickness;
  wkttool::Color color;
  bool hovered;
  bool deleted;
  bool editing;
};

struct BadLine {
  std::string error;
  std::string line;
};

struct ImportResult {
  std::vector<Shape> shapes;
  std::vector<BadLine> bad_lines;
};

std::string generate_label() {
  static int counter = 0;
  return "shape" + std::to_string(counter++);
}

wkttool::Color random_color() {
  return wkttool::Color{
    wkttool::Red{static_cast<uint8_t>(rand()%255)},
    wkttool::Green{static_cast<uint8_t>(rand()%255)},
    wkttool::Blue{static_cast<uint8_t>(rand()%255)}};
}

ImportResult import_shapes(const std::string& raw) {
  using namespace wkttool;
  ImportResult result;
  const auto tokens = tokenize(raw);
  for (const auto& token : tokens) {
    if (token.empty()) continue;
    try {
      const auto geo = parse(token);
      if (geo) {
        result.shapes.push_back(Shape{generate_label(), *geo, Thickness{4}, random_color()});
      } else {
        result.bad_lines.push_back({"unidentified object type " + token, token});
      }
    } catch (const std::exception& ex) {
        std::stringstream error;
        error << "Exception: " << ex.what() << " for " << token;
        result.bad_lines.push_back({error.str(), token});
    }
  }
  return result;
}

void draw_shapes(const std::vector<Shape> shapes, wkttool::SFMLWindowAdapter& window,
wkttool::ScreenProjection& proj) {
  using namespace wkttool;
  for (const auto& shape : shapes) {
        const Thickness actual_thickness{
          shape.thickness.get() * (shape.hovered ? 2 : 1)};
        std::visit(overload{
          [&] (const geometry::Polygon& poly) {
              const auto segments =  to_segments(poly);
              window.draw(segments_to_drawables(segments, proj, shape.color, actual_thickness));},
          [&] (const geometry::MultiPolygon& polys) {
              const auto segments =  to_segments(polys);
              window.draw(segments_to_drawables(segments, proj, shape.color, actual_thickness));},
          [&] (const geometry::MultiLinestring& lss) {
              const auto segments =  to_segments(lss);
              window.draw(segments_to_drawables(segments, proj, shape.color, actual_thickness));},
          [&] (const geometry::Linestring& ls) {
              const auto segments =  to_segments(ls);
              window.draw(segments_to_drawables(segments, proj, shape.color, actual_thickness));},
          [&] (const geometry::Point& point) {
              window.draw(point_to_drawables(
              point, proj, shape.color, actual_thickness, Right{10}, Down{10}));},
          [&] (const geometry::MultiPoint& points) {
              window.draw(points_to_drawables(
              points, proj, shape.color, actual_thickness, Right{10}, Down{10}));}
            }, shape.geometry);

  }
}

void draw(wkttool::SFMLWindowAdapter &window,
          const wkttool::geometry::Point &center, const float scale,
          const wkttool::ScreenDimensions &dims, const std::vector<Shape>& shapes) {
  using namespace wkttool;
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
  draw_shapes(shapes, window, proj);

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
  std::string editor {initial_scenario};
  std::string errors;
  std::vector<Shape> shapes;
  while (running) {
    window.handle_events();
    draw(window, center, scale, dims, shapes);
    const auto new_frame = std::chrono::system_clock::now();
    std::stringstream framestring;
    framestring << "Frame Time: " << (new_frame - last_frame).count() / 1e6;

    last_frame = new_frame;
    ImGui::Begin("Control");
    ImGui::GetClipboardText();
    ImGui::Text(framestring.str().c_str());
    ImGui::InputTextMultiline("##Editor", &editor);
    is_window_hovered = ImGui::IsWindowHovered() or ImGui::IsWindowFocused();
    if (ImGui::Button("Import")) {
      const auto result = import_shapes(editor);
      editor.clear();
      errors.clear();
      std::copy(std::begin(result.shapes), std::end(result.shapes), 
                std::back_inserter(shapes));
      std::stringstream errors_stream;
      std::stringstream editor_stream;
      for (const auto& bad_line : result.bad_lines){
        errors_stream << bad_line.error << "\n";
        editor_stream << bad_line.line << ";\n";
      }
      editor = editor_stream.str();
      errors = errors_stream.str();
    }
    ImGui::Text("Errors:");
    ImGui::Text(errors.c_str());
    for (auto& shape : shapes) {
      ImGui::BeginGroup();
      ImGui::Text(shape.label.c_str());
      ImGui::SameLine();
      if (ImGui::Button((std::string{"Delete##"} + shape.label).c_str())) {
        shape.deleted = true;
      }
      ImGui::SameLine();
      if (ImGui::Button((std::string{"Edit##"} + shape.label).c_str())) {
        shape.editing = true;
      }
      ImGui::EndGroup();
      if (ImGui::IsItemHovered()) {
        shape.hovered = true;
      } else {
        shape.hovered = false;
      }
    }
    auto it = std::begin(shapes);
    while (it != std::end(shapes))
    {
      if (it->deleted){
        it = shapes.erase(it);
      } else if (it->editing) {
        std::stringstream as_wkt;
        as_wkt << ";\n";
        std::visit( [&as_wkt](const auto& g) {
            as_wkt << boost::geometry::wkt(g);}, 
          it->geometry);
        editor += as_wkt.str();
        it = shapes.erase(it);
      }else{
        ++it;
        }
    }
    ImGui::End();
    window.display();
  }

  return EXIT_SUCCESS;
}
