#include "Box2dWorld.h"

#include <cassert>

Box2dWorld::Box2dWorld() {
  const float WorldWidthMetres = 128.0f;
  const float WorldHeightMetres = 96.0f;

  b2WorldDef worldDef = b2DefaultWorldDef();
  worldDef.gravity = (b2Vec2){0.0f, 0.0f};
  worldId = b2CreateWorld(&worldDef);

  b2BodyDef groundBodyDef = b2DefaultBodyDef();
  groundBodyDef.position = (b2Vec2){0, 0};
  groundId = b2CreateBody(worldId, &groundBodyDef);
  wallsId = b2CreateBody(worldId, &groundBodyDef);

  b2Polygon bottomBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){WorldWidthMetres / 2.0f, -WorldHeightMetres / 2.0f}, b2MakeRot(0.0f));
  b2Polygon leftBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){-WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f}, b2MakeRot(0.0f));
  b2Polygon rightBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){WorldWidthMetres * 1.5f, WorldHeightMetres / 2.0f}, b2MakeRot(0.0f));
  b2Polygon topBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){WorldWidthMetres / 2.0f, WorldHeightMetres * 1.5f}, b2MakeRot(0.0f));

  b2ShapeDef groundShapeDef = b2DefaultShapeDef();
  b2CreatePolygonShape(groundId, &groundShapeDef, &bottomBox);
  b2CreatePolygonShape(groundId, &groundShapeDef, &leftBox);
  b2CreatePolygonShape(groundId, &groundShapeDef, &rightBox);
  b2CreatePolygonShape(groundId, &groundShapeDef, &topBox);

  b2Polygon box1 = b2MakeOffsetBox(5, 5, (b2Vec2){30.0f, 30.0f}, b2MakeRot(0.0f));
  b2Polygon box2 = b2MakeOffsetBox(5, 5, (b2Vec2){60.0f, 60.0f}, b2MakeRot(45.0f));
  b2CreatePolygonShape(wallsId, &groundShapeDef, &box1);
  b2CreatePolygonShape(wallsId, &groundShapeDef, &box2);
};

std::vector<b2ShapeId> Box2dWorld::getStaticItems() {
  int shapeCount = b2Body_GetShapeCount(wallsId);
  std::vector<b2ShapeId> shapeIds(shapeCount);
  int returnCount = b2Body_GetShapes(wallsId, shapeIds.data(), shapeCount);
  assert(shapeCount == returnCount);
  return shapeIds;
}