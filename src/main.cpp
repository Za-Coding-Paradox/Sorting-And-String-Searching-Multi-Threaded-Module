#include "controller.h"

auto main(int argc, char *argv[]) -> int {
  mth_css::Controller controller;
  if (!controller.initialize()) {
    return 1;
  }
  controller.runMenu();
  return 0;
}
