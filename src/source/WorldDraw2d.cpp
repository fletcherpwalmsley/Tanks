#include "WorldDraw2d.h"

#include <grrlib.h>
#include <math.h>

#include <cassert>

WorldDraw2d::WorldDraw2d() { GRRLIB_Init(); }

WorldDraw2d::~WorldDraw2d() { GRRLIB_Exit(); }

void WorldDraw2d::NewFrame() {
  GRRLIB_Render();
  GRRLIB_FillScreen(0xFFFFFFFF);
}

void WorldDraw2d::DrawRotatedRectangle(float cx, float cy, float width, float height, float radians, uint32_t color) {
  // Half the width and height for easier calculations

  cx *= WorldToWindowScale;
  cy *= WorldToWindowScale;
  cy = 480 - cy;
  width *= WorldToWindowScale;
  height *= WorldToWindowScale;

  float half_width = width / 2;
  float half_height = height / 2;

  // Calculate the positions of the 4 corners of the rectangle
  float corners[4][2] = {
      {cx - half_width, cy + half_height},  // Top-left corner
      {cx + half_width, cy + half_height},  // Top-right corner
      {cx + half_width, cy - half_height},  // Bottom-right corner
      {cx - half_width, cy - half_height}   // Bottom-left corner
  };

  // Rotate each corner around the center (cx, cy)
  for (int i = 0; i < 4; ++i) {
    float x = corners[i][0] - cx;
    float y = corners[i][1] - cy;

    // Apply rotation matrix
    float new_x = x * cos(radians) - y * sin(radians) + cx;
    float new_y = x * sin(radians) + y * cos(radians) + cy;

    corners[i][0] = new_x;
    corners[i][1] = new_y;
  }

  // Draw the lines connecting the rotated corners
  GRRLIB_Line(corners[0][0], corners[0][1], corners[1][0], corners[1][1], color);  // Top edge
  GRRLIB_Line(corners[1][0], corners[1][1], corners[2][0], corners[2][1], color);  // Right edge
  GRRLIB_Line(corners[2][0], corners[2][1], corners[3][0], corners[3][1], color);  // Bottom edge
  GRRLIB_Line(corners[3][0], corners[3][1], corners[0][0], corners[0][1], color);  // Left edge
};

void WorldDraw2d::DrawId(b2ShapeId& id) {
  switch (b2Shape_GetType(id)) {
    case b2_polygonShape:
      Draw(b2Shape_GetPolygon(id));
      break;

    default:
      assert(false);
      break;
  }
};

void WorldDraw2d::Draw(b2Polygon polygon) {
  if (polygon.count > 2) {
    for (int i = 0; i < polygon.count - 1; i++) {
      GRRLIB_Line(polygon.vertices[i].x * WorldToWindowScale, polygon.vertices[i].y * WorldToWindowScale, polygon.vertices[i + 1].x * WorldToWindowScale,
                  polygon.vertices[i + 1].y * WorldToWindowScale, 0x800080FF);
    }
    // Handle the final line back to the start
    GRRLIB_Line(polygon.vertices[polygon.count - 1].x * WorldToWindowScale, polygon.vertices[polygon.count - 1].y * WorldToWindowScale, polygon.vertices[0].x * WorldToWindowScale,
                polygon.vertices[0].y * WorldToWindowScale, 0x800080FF);
  }
}