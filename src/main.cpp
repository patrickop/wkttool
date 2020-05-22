#include <wkttool/sfml_window_adapter.h>

int main(int, char**) {
  using namespace wkttool;
  SFMLWindowAdapter window{"my window", 
    ScreenDimensions{ Right{800}, Down{600}}};
  
  bool running = true;
  auto event_listener = [&running] 
    (const auto&) {
      running = false;
  };

  window.connect(event_listener);
  shape::Segment s{
    ScreenLocation{Right{300}, Down{300}},
    ScreenLocation{Right{500}, Down{300}}};
  while (running) {
    window.handle_events();
    window.clear();
    window.draw(s);
    window.display();
  }

  return EXIT_SUCCESS;

}
