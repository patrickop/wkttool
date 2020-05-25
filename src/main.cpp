#include <wkttool/sfml_window_adapter.h>
int main(int, char **) {
  using namespace wkttool;
  SFMLWindowAdapter window{"my window",
                           ScreenDimensions{Right{800}, Down{600}}};

  bool running = true;
  window.connect([&running](const WindowClosed &) { running = false; });
  window.connect([&running](const MouseMoved &ev) {
    std::cout << "Right " << ev.destination.right.get()
              << " "
                 "Left "
              << ev.destination.down.get() << std::endl;
  });
  shape::Segment s{black,
                   Thickness{5},
                   {ScreenLocation{Right{300}, Down{300}},
                    ScreenLocation{Right{500}, Down{400}}}};
  while (running) {
    window.handle_events();
    window.clear(white);
    window.draw(s);
    window.display();
  }

  return EXIT_SUCCESS;
}
