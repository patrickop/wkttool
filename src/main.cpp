#include <wkttool/make_grid.h>
#include <wkttool/screen_projection.h>
#include <wkttool/sfml_window_adapter.h>
int main(int, char **) {
  using namespace wkttool;
  const ScreenDimensions dims{Right{800}, Down{600}};
  CoordinateBoundaries bounds{LowerXBoundary{-10}, LowerYBoundary{-10},
                              UpperXBoundary{10}, UpperYBoundary{10}};

  ScreenProjection proj{dims, bounds};

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
  const auto grid =
      make_grid(bounds, proj, XStep{2.0}, YStep{2.0}, black, grey);

  while (running) {
    window.handle_events();
    window.clear(white);
    window.draw(grid);
    window.display();
  }

  return EXIT_SUCCESS;
}
