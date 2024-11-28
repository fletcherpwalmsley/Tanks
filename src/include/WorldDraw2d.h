#pragma once

#include <box2d/box2d.h>

#include <cstdint>
class WorldDraw2d {
 public:
  WorldDraw2d();
  ~WorldDraw2d();
  void NewFrame();
  void DrawRotatedRectangle(float cx, float cy, float width, float height, float radians, uint32_t color);
  void DrawId(b2ShapeId& polygon);

 private:
  const float WorldToWindowScale = 5.0f;
  void Draw(b2Polygon polygon);
};
