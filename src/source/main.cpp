#include <iostream>

#include "Box2dWorld.h"
#include "WorldDraw2d.h"

using std::cout;

int main(int argc, char **argv) {
  Box2dWorld sim;
  WorldDraw2d draw;
  // sim.createBox(60, 55);
  while (true) {
    sim.Step();
    draw.NewFrame();
    for (auto id : sim.getStaticItems()) {
      draw.DrawId(id);
    }
    for (auto id : sim.getDynamicItems()) {
      draw.DrawId(id);
    }
  }
  exit(0);
}