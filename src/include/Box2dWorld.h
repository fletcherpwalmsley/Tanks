#pragma once

#include <box2d/box2d.h>

#include <vector>

class Box2dWorld {
 public:
  Box2dWorld();
  void Step();
  auto getStaticItems() -> std::vector<b2ShapeId>;
  auto getDynamicItems() -> std::vector<b2ShapeId>;
  // auto getDynamicItems() -> b2ShapeId;
  void createBox(int x, int y);

 private:
  b2WorldId m_worldId;
  b2BodyId m_groundId;
  b2BodyId m_wallsId;
  std::vector<b2BodyId> m_dynamicIds;
  b2BodyId tempId;
};
