#pragma once

#include <box2d/box2d.h>

#include <vector>

class Box2dWorld {
 public:
  Box2dWorld();
  void Step() { b2World_Step(worldId, 1 / 60, 4); }
  std::vector<b2ShapeId> getStaticItems();

 private:
  b2WorldId worldId;
  b2BodyId groundId;
  b2BodyId wallsId;
};
