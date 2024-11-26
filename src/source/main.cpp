#include <iostream>

#include "Box2dWorld.h"
#include "WorldDraw2d.h"

using std::cout;

int main(int argc, char **argv) {
  Box2dWorld sim;
  WorldDraw2d draw;
  while (1) {
    sim.Step();
    draw.NewFrame();
    for (auto id : sim.getStaticItems()) {
      draw.DrawId(id);
    }
  }
  exit(0);
}