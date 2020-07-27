#include <wkttool/make_grid.h>
#include <wkttool/screen_projection.h>
#include <wkttool/sfml_window_adapter.h>
#include <wkttool/subsample.h>
#include <wkttool/to_segments.h>
#include <wkttool/parse_geometry.h>
#include <wkttool/label_position.h>
#include <imgui.h>
#include <imgui_stdlib.h>
constexpr unsigned max_gridlines = 50;
constexpr float min_coverage = 1e-5;
constexpr auto initial_scenario = "POINT(1 2);\n\
LINESTRING(0 0,2 2,3 1);\n\
POLYGON((0 0,0 7,4 2,2 0,0 0));\n\
MULTIPOINT(1.5 2.5, -3 -2);\n\
MULTILINESTRING((-1 2, -2 1),(7 7, 8 10));\n\
MULTIPOLYGON(((0 0,0 -7,-4 -2,-2 0,0 0)), ((5 -2, 6 -3, 4 -2.5, 5 -2)))";
//template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
//template<class... Ts> overload(Ts...) -> overload<Ts...>;
using ImguiColor = std::array<float, 3>;
struct Shape {
  std::string label;
  wkttool::geometry::Geometry geometry;
  wkttool::Thickness thickness;
  bool hovered;
  bool deleted;
  bool editing;
  ImguiColor imgui_color;
};



struct BadLine {
  std::string error;
  std::string line;
};

struct ImportResult {
  std::vector<Shape> shapes;
  std::vector<BadLine> bad_lines;
};

struct GuiState {
  std::vector<Shape> shapes;
  ImguiColor background{1,1,1};
  float center_x = 0;
  float center_y = 0;
  float coverage_x = 10;
  float coverage_y = 10;
  float grid_x = 2;
  float grid_y = 2;
  std::string editor_content = initial_scenario;
  std::vector<std::string> errors;
};

void fix_impossibilities(GuiState& state) {
  if (state.coverage_x == 0) state.coverage_x = min_coverage;
  if (state.coverage_y == 0) state.coverage_y = min_coverage;
  state.grid_x = std::fabs(state.grid_x);
  state.grid_y = std::fabs(state.grid_y);
  state.coverage_x = std::fabs(state.coverage_x);
  state.coverage_y = std::fabs(state.coverage_y);
  if (state.coverage_x / state.grid_x > max_gridlines) {
    state.grid_x = state.coverage_x / (max_gridlines-1);
  }
  if (state.coverage_y / state.grid_y > max_gridlines) {
    state.grid_y = state.coverage_y / (max_gridlines-1);
  }
}

wkttool::Color to_color(const ImguiColor& imgui_color) {
  return wkttool::Color{
    wkttool::Red { static_cast<uint8_t>((255.0 * std::get<0>(imgui_color)))},
    wkttool::Green { static_cast<uint8_t>((255.0 * std::get<1>(imgui_color)))},
    wkttool::Blue { static_cast<uint8_t>((255.0 * std::get<2>(imgui_color)))}};
}

std::string generate_label() {
  static int counter = 0;
  return "shape" + std::to_string(counter++);
}

ImguiColor random_imgui_color() {
  return {
    ((float) rand() / (RAND_MAX)),
    ((float) rand() / (RAND_MAX)),
    ((float) rand() / (RAND_MAX))};

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
        result.shapes.push_back(Shape{generate_label(), *geo, Thickness{4}, false, false, false, random_imgui_color()});
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
              window.draw(segments_to_drawables(segments, proj, to_color(shape.imgui_color), actual_thickness));},
          [&] (const geometry::MultiPolygon& polys) {
              const auto segments =  to_segments(polys);
              window.draw(segments_to_drawables(segments, proj, to_color(shape.imgui_color), actual_thickness));},
          [&] (const geometry::MultiLinestring& lss) {
              const auto segments =  to_segments(lss);
              window.draw(segments_to_drawables(segments, proj, to_color(shape.imgui_color), actual_thickness));},
          [&] (const geometry::Linestring& ls) {
              const auto segments =  to_segments(ls);
              window.draw(segments_to_drawables(segments, proj, to_color(shape.imgui_color), actual_thickness));},
          [&] (const geometry::Point& point) {
              window.draw(point_to_drawables(
              point, proj, to_color(shape.imgui_color), actual_thickness, Right{10}, Down{10}));},
          [&] (const geometry::MultiPoint& points) {
              window.draw(points_to_drawables(
              points, proj, to_color(shape.imgui_color), actual_thickness, Right{10}, Down{10}));}
            }, shape.geometry);
    window.draw(text_to_drawable(shape.label, label_position(shape.geometry), proj, black, PointSize{22}));
  }
}
wkttool::CoordinateBoundaries get_bounds(const GuiState& gui_state) {
  using namespace wkttool;
  return CoordinateBoundaries {
           LowerXBoundary{gui_state.center_x - (gui_state.coverage_x/2)},
           LowerYBoundary{gui_state.center_y - (gui_state.coverage_y/2)},
           UpperXBoundary{gui_state.center_x + (gui_state.coverage_x/2)},
           UpperYBoundary{gui_state.center_y + (gui_state.coverage_y/2)}};
}
void draw_scene(wkttool::SFMLWindowAdapter &window, const GuiState& gui_state,
          const wkttool::ScreenDimensions &dims) {
  using namespace wkttool;
  window.clear(to_color(gui_state.background));
  const auto bounds = get_bounds(gui_state);
  ScreenProjection proj{dims, bounds};
  const auto grid = make_grid(bounds, XStep{gui_state.grid_x}, YStep{gui_state.grid_y});
  const auto axes = make_axes(bounds);
  window.draw(segments_to_drawables(grid, proj, grey, Thickness{1}));
  window.draw(segments_to_drawables(axes, proj, black, Thickness{2}));
  draw_shapes(gui_state.shapes, window, proj);

}
void draw_gui(GuiState& gui_state) {
    ImGui::Begin("Window Control");
    ImGui::InputFloat("CenterX", &gui_state.center_x);
    ImGui::InputFloat("CenterY", &gui_state.center_y);
    ImGui::InputFloat("CoverageX", &gui_state.coverage_x);
    ImGui::InputFloat("CoverageY", &gui_state.coverage_y);
    ImGui::InputFloat("GridX", &gui_state.grid_x);
    ImGui::InputFloat("GridY", &gui_state.grid_y);
    ImGui::ColorEdit3("Background", (float*)gui_state.background.data(), ImGuiColorEditFlags_NoInputs );
    ImGui::End();
    ImGui::Begin("Scene");
    ImGui::InputTextMultiline("##Editor", &gui_state.editor_content);
    if (ImGui::Button("Import")) {
      const auto result = import_shapes(gui_state.editor_content);
      gui_state.editor_content.clear();
      gui_state.errors.clear();
      std::copy(std::begin(result.shapes), std::end(result.shapes), 
                std::back_inserter(gui_state.shapes));
      std::stringstream editor_stream;
      for (const auto& bad_line : result.bad_lines){
        gui_state.errors.push_back(bad_line.error);
        editor_stream << bad_line.line << ";\n";
      }
      gui_state.editor_content = editor_stream.str();
    }
    ImGui::Text("Errors:");
    for (const auto& error : gui_state.errors) {
      ImGui::Text(error.c_str());
    }
    for (auto& shape : gui_state.shapes) {
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
      ImGui::SameLine();
      ImGui::ColorEdit3((std::string{"##Color"} + shape.label).c_str(), (float*)shape.imgui_color.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel );
      ImGui::EndGroup();
      if (ImGui::IsItemHovered()) {
        shape.hovered = true;
      } else {
        shape.hovered = false;
      }
    }
    auto it = std::begin(gui_state.shapes);
    while (it != std::end(gui_state.shapes))
    {
      if (it->deleted){
        it = gui_state.shapes.erase(it);
      } else if (it->editing) {
        std::stringstream as_wkt;
        as_wkt << "\n";
        std::visit( [&as_wkt](const auto& g) {
            as_wkt << boost::geometry::wkt(g);}, 
          it->geometry);
        as_wkt << ";";
        gui_state.editor_content += as_wkt.str();
        it = gui_state.shapes.erase(it);
      }else{
        ++it;
        }
    }
    ImGui::End();
}

int main(int, char **) {
  using namespace wkttool;
  const ScreenDimensions dims{Right{800}, Down{600}};

  SFMLWindowAdapter window{"my window",
                           ScreenDimensions{Right{800}, Down{600}}};

  bool running = true;
  float scale = 1.0;
  bool redraw = true;
  bool disable_mouse = false;
  GuiState gui_state;
  std::optional<ScreenLocation> mouse_pressed_down_at = std::nullopt;
  geometry::Point center{0, 0};
  window.connect([&running](const WindowClosed &) { running = false; });
  window.connect([&gui_state, &redraw, &disable_mouse](const MouseWheelScrolled &ev) {
    if (disable_mouse) return;
    gui_state.coverage_x *= std::pow(1.20, -ev.amount.get());
    gui_state.coverage_y *= std::pow(1.20, -ev.amount.get());
    redraw = true;
  });
  window.connect([&mouse_pressed_down_at, &disable_mouse](const MouseButtonDown &ev) {
    mouse_pressed_down_at = ev.location;
  });
  window.connect([&mouse_pressed_down_at, &disable_mouse](const MouseButtonUp &ev) {
    mouse_pressed_down_at = std::nullopt;
  });
  window.connect([&mouse_pressed_down_at, &redraw, &gui_state,
                  &dims, &disable_mouse](const MouseMoved &ev) {
    if (disable_mouse) return;
    if (mouse_pressed_down_at) {
      const auto shift = *mouse_pressed_down_at - ev.destination;
      const auto bounds = get_bounds(gui_state);
      ScreenProjection proj{dims, bounds};
      const auto center = proj.translate(geometry::Point{gui_state.center_x, gui_state.center_y}, shift);
      gui_state.center_x = boost::geometry::get<0>(center);
      gui_state.center_y = boost::geometry::get<1>(center);
      mouse_pressed_down_at = ev.destination;
      redraw = true;
    }
  });
  auto last_frame = std::chrono::system_clock::now();
  std::string editor {initial_scenario};
  std::string errors;
  std::vector<Shape> shapes;
  float x = 0;
  float y = 0;
  while (running) {
    window.handle_events();
    fix_impossibilities(gui_state);
    draw_scene(window, gui_state, dims);
    const auto new_frame = std::chrono::system_clock::now();
    std::stringstream framestring;
    framestring << "Frame Time: " << (new_frame - last_frame).count() / 1e6;
    ImGui::GetClipboardText();

    draw_gui(gui_state);
    disable_mouse = ImGui::GetIO().WantCaptureMouse;
    last_frame = new_frame;
    window.display();
  }

  return EXIT_SUCCESS;
}
