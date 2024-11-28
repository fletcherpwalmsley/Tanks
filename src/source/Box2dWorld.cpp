#include "Box2dWorld.h"

#include <cassert>
#include <vector>

Box2dWorld::Box2dWorld() {
  const float WorldWidthMetres = 128.0f;
  const float WorldHeightMetres = 96.0f;

  b2WorldDef worldDef = b2DefaultWorldDef();
  worldDef.gravity = (b2Vec2){0.0f, -10.0f};
  m_worldId = b2CreateWorld(&worldDef);

  b2BodyDef groundBodyDef = b2DefaultBodyDef();
  groundBodyDef.position = (b2Vec2){0, 0};
  m_groundId = b2CreateBody(m_worldId, &groundBodyDef);
  m_wallsId = b2CreateBody(m_worldId, &groundBodyDef);

  b2Polygon bottomBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){WorldWidthMetres / 2.0f, -WorldHeightMetres / 2.0f}, b2MakeRot(0.0f));
  b2Polygon leftBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){-WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f}, b2MakeRot(0.0f));
  b2Polygon rightBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){WorldWidthMetres * 1.5f, WorldHeightMetres / 2.0f}, b2MakeRot(0.0f));
  b2Polygon topBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){WorldWidthMetres / 2.0f, WorldHeightMetres * 1.5f}, b2MakeRot(0.0f));

  b2ShapeDef groundShapeDef = b2DefaultShapeDef();
  b2CreatePolygonShape(m_groundId, &groundShapeDef, &bottomBox);
  b2CreatePolygonShape(m_groundId, &groundShapeDef, &leftBox);
  b2CreatePolygonShape(m_groundId, &groundShapeDef, &rightBox);
  b2CreatePolygonShape(m_groundId, &groundShapeDef, &topBox);

  b2Polygon box1 = b2MakeOffsetBox(5, 5, (b2Vec2){30.0f, 30.0f}, b2MakeRot(0.0f));
  b2Polygon box2 = b2MakeOffsetBox(5, 5, (b2Vec2){60.0f, 60.0f}, b2MakeRot(45.0f));
  b2CreatePolygonShape(m_wallsId, &groundShapeDef, &box1);
  b2CreatePolygonShape(m_wallsId, &groundShapeDef, &box2);
  createBox(0, 0);
};

auto Box2dWorld::getStaticItems() -> std::vector<b2ShapeId> {
  int shapeCount = b2Body_GetShapeCount(m_wallsId);
  std::vector<b2ShapeId> shapeIds(shapeCount);
  int returnCount = b2Body_GetShapes(m_wallsId, shapeIds.data(), shapeCount);
  assert(shapeCount == returnCount);
  return shapeIds;
}

auto Box2dWorld::getDynamicItems() -> std::vector<b2ShapeId> {
  // Work out how large the return std::vector needs to be
  // std::vector<b2ShapeId> retVect(5);
  // for (auto id : m_dynamicIds) {
  int shapeCount = b2Body_GetShapeCount(tempId);
  std::vector<b2ShapeId> shapeIds(shapeCount);
  int returnCount = b2Body_GetShapes(tempId, shapeIds.data(), shapeCount);
  assert(shapeCount == returnCount);
  // for (auto item : shapeIds) {
  //   retVect.push_back(item);
  // }
  // }
  return shapeIds;  // Returning a big object. Might need to improve this
}

void Box2dWorld::createBox(int x, int y) {
  // Define the box type (Dynamic)
  b2BodyDef boxDef = b2DefaultBodyDef();
  boxDef.type = b2_dynamicBody;
  boxDef.position = (b2Vec2){0.0f, 0.0f};
  boxDef.rotation = b2MakeRot(45.0f);
  b2BodyId bodyId = b2CreateBody(m_worldId, &boxDef);

  // Define the box shape
  b2Polygon dynamicBox = b2MakeOffsetBox(5, 5, (b2Vec2){70.0f, 40.0f}, b2MakeRot(-45.0f));
  b2ShapeDef boxShape = b2DefaultShapeDef();
  boxShape.density = 1.0f;
  boxShape.friction = 0.3f;
  b2CreatePolygonShape(bodyId, &boxShape, &dynamicBox);
  m_dynamicIds.push_back(bodyId);
  tempId = bodyId;
}

void Box2dWorld::Step() { b2World_Step(m_worldId, 1.0f / 60.0f, 4); }