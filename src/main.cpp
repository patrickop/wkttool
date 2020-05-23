#include <wkttool/sfml_window_adapter.h>
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
int main(int, char**) {
  using namespace wkttool;
  SFMLWindowAdapter window{"my window", 
    ScreenDimensions{ Right{800}, Down{600}}};
  
  bool running = true;
  //auto event_listener = overloaded{
  //  [&running] (const WindowClosed&) { running = false; },
  //  [] (const MouseMoved& mouse_moved) {
  //    std::cout << mouse_moved.position.right.get() << "," 
  //                 mouse_moved.position.down.get() << std::endl; }
  //                 };
  //window.connect(event_listener);
  shape::Segment s{
    black,
    Thickness{5},
    ScreenLocation{Right{300}, Down{300}},
    ScreenLocation{Right{500}, Down{400}}};
  while (running) {
    window.handle_events();
    window.clear(white);
    window.draw(s);
    window.display();
  }

  return EXIT_SUCCESS;

}
