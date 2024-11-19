/*===========================================
                  Tanks?
            - ball_click_spawn -

          Builds on box_click_spawn.
          Spawns a ball on click. Into a
            world with no gravity
        The idea with this example is to 
        test potential bullet bouncing
============================================*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <vector>
#include <random>

#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <box2d/box2d.h>

#include "crosshair_png.h"

static const float WorldWidthMetres = 128.0f; 
static const float WorldHeightMetres = 96.0f;
static const float BoxLengthMetres = 5.0f;
static const float WorldToWindowScale = 5.0f; // Gives 640*480 with 128*96 world values
static b2WorldId worldId;

struct boxInfo {
    b2BodyId  body;
    u_int32_t colour;
};


u_int32_t& RandomColour() {
    static std::random_device rd;  // a seed source for the random number engine
    static std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    static std::vector<u_int32_t> colours = {
        0x000000FF, 0x800000FF, 0x008000FF, 0x808000FF, 0x000080FF, 0x800080FF, 0x008080FF, 0x808080FF, 0xC0C0C0FF,
        0xFF0000FF, 0x00FF00FF, 0xFFFF00FF, 0x0000FFFF, 0xFF00FFFF, 0x00FFFFFF
    };
    static std::uniform_int_distribution<> distrib(0, colours.size()-1);
    return colours[distrib(gen)];
}


void DrawRotatedRectangle(float cx, float cy, float width, float height, float radians, u32 color) {
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
        {cx - half_width, cy + half_height}, // Top-left corner
        {cx + half_width, cy + half_height}, // Top-right corner
        {cx + half_width, cy - half_height}, // Bottom-right corner
        {cx - half_width, cy - half_height}  // Bottom-left corner
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
    GRRLIB_Line(corners[0][0], corners[0][1], corners[1][0], corners[1][1], color); // Top edge
    GRRLIB_Line(corners[1][0], corners[1][1], corners[2][0], corners[2][1], color); // Right edge
    GRRLIB_Line(corners[2][0], corners[2][1], corners[3][0], corners[3][1], color); // Bottom edge
    GRRLIB_Line(corners[3][0], corners[3][1], corners[0][0], corners[0][1], color); // Left edge
}


b2BodyId MakeBall(float x, float y) {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){x, y};
    bodyDef.linearVelocity = (b2Vec2){-30.0f, -30.0f};
    // bodyDef.angularVelocity = -3.14f;


    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Circle circle;
    circle.center = (b2Vec2){0.0f, 0.0f};
    circle.radius = 2.5;
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.1f;
    shapeDef.restitution = 1.0f;
    b2CreateCircleShape(bodyId, &shapeDef, &circle);
    return bodyId;
}

int main(int argc, char **argv) {
    GRRLIB_Init();
    WPAD_Init();

    GRRLIB_texImg* crosshair  = GRRLIB_LoadTexturePNG(crosshair_png);
    GRRLIB_SetMidHandle(crosshair, true);

    // Initialise the Wiimotes
    const u16 WinW = rmode->fbWidth;
    const u16 WinH = rmode->efbHeight;
    ir_t P1Mote;
    WPAD_SetIdleTimeout(60 * 10);
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    
    // Setup the Box2d world
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};
    worldId = b2CreateWorld(&worldDef);
    
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){0, 0};
    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2Polygon bottomBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){WorldWidthMetres / 2.0f, -WorldHeightMetres / 2.0f}, b2MakeRot(0.0f));
    b2Polygon leftBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){-WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f}, b2MakeRot(0.0f));
    b2Polygon rightBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){WorldWidthMetres * 1.5f, WorldHeightMetres / 2.0f}, b2MakeRot(0.0f));
    b2Polygon topBox = b2MakeOffsetBox(WorldWidthMetres / 2.0f, WorldHeightMetres / 2.0f, (b2Vec2){WorldWidthMetres / 2.0f, WorldHeightMetres * 1.5f}, b2MakeRot(0.0f));
    
    b2CreatePolygonShape(groundId, &groundShapeDef, &bottomBox);
    b2CreatePolygonShape(groundId, &groundShapeDef, &leftBox);
    b2CreatePolygonShape(groundId, &groundShapeDef, &rightBox);
    b2CreatePolygonShape(groundId, &groundShapeDef, &topBox);

    std::vector<boxInfo> boxes;
    float timeStep = 1.0f / 60.0f;
    int subStepCount = 4;
    while(1) {
        WPAD_ScanPads();  // Scan the Wiimotes
        u32 WPADKeyDown = WPAD_ButtonsDown(WPAD_CHAN_0);
        WPAD_SetVRes(WPAD_CHAN_0, WinW, WinH);
        WPAD_IR(WPAD_CHAN_0, &P1Mote);

        GRRLIB_SetBlend(GRRLIB_BLEND_ALPHA);

        // Wii Remote IR Viewport correction
        int P1MX = P1Mote.sx - 150;
        int P1MY = P1Mote.sy - 150;

        // See if we need to add a new box!
        if (WPADKeyDown & WPAD_BUTTON_B) {
            boxInfo box;
            box.body = MakeBall((P1MX) / WorldToWindowScale, (480 - P1MY) / WorldToWindowScale);
            box.colour = RandomColour();
            boxes.push_back(box);
        }

        // If [HOME] was pressed on the first Wiimote, break out of the loop
        if (WPADKeyDown & WPAD_BUTTON_HOME)  break;

        b2World_Step(worldId, timeStep, subStepCount);

        // Draw code
        GRRLIB_FillScreen(0xFFFFFFFF);
        for (auto box : boxes) {
            b2Vec2 position = b2Body_GetPosition(box.body);
            b2Vec2 linearVelocity = b2Body_GetLinearVelocity(box.body);
            b2Vec2 directionUnitVector = b2Normalize(linearVelocity);

            GRRLIB_Circle(
                position.x*WorldToWindowScale,
                480 - position.y*WorldToWindowScale,
                2.5*WorldToWindowScale,
                box.colour,
                1
            );
            GRRLIB_Line(
                position.x * WorldToWindowScale,
                480 - position.y * WorldToWindowScale,
                (position.x * WorldToWindowScale) + (directionUnitVector.x * 40),
                (480 - position.y * WorldToWindowScale) - (directionUnitVector.y * 40),
                box.colour); // Top edge
        }
        // Draw Crosshair
        GRRLIB_DrawImg(P1MX, P1MY, crosshair, 0, 1, 1, RGBA(255, 255, 255, 255));
        GRRLIB_Render();  // Render the frame buffer to the TV
    }
    GRRLIB_FreeTexture(crosshair);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}
