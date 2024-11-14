/*===========================================
                  Tanks?
               - box_bounce -

    Builds on hello_box2d.
    Visualises the box and create a wall 
    around the screen to keep the box in view. 

    Font taken from GRRLIB bitmap_fx example
============================================*/
#include <stdlib.h>
#include <stdio.h>
#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <box2d/box2d.h>

#include <math.h>

#include "font1_png.h"

// RGBA Colors
#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_MAROON  0x800000FF
#define GRRLIB_GREEN   0x008000FF
#define GRRLIB_OLIVE   0x808000FF
#define GRRLIB_NAVY    0x000080FF
#define GRRLIB_PURPLE  0x800080FF
#define GRRLIB_TEAL    0x008080FF
#define GRRLIB_GRAY    0x808080FF
#define GRRLIB_SILVER  0xC0C0C0FF
#define GRRLIB_RED     0xFF0000FF
#define GRRLIB_LIME    0x00FF00FF
#define GRRLIB_YELLOW  0xFFFF00FF
#define GRRLIB_BLUE    0x0000FFFF
#define GRRLIB_FUCHSIA 0xFF00FFFF
#define GRRLIB_AQUA    0x00FFFFFF
#define GRRLIB_WHITE   0xFFFFFFFF

const float WorldToWindowScale = 5.0f; // Gives 640*480 with 128*96 world values

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


int main(int argc, char **argv) {

    // Set-up font
    GRRLIB_texImg *text_font1 = GRRLIB_LoadTexture(font1_png);
    GRRLIB_InitTileSet(text_font1, 32, 36, 32);

    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();

    // Initialise the Wiimotes
    WPAD_Init();

    // Work out some units!
    const float WorldWidthMetres = 128.0f; 
    const float WorldHeightMetres = 96.0f;
    
    const float SquareLengthMetres = 5.0f;

    // Rework these to handle NTSC vert 
    // const float WorldToWindowWidthScale = rmode->fbWidth / WorldWidthMetres;
    // const float WorldToWindowHeightScale = rmode->efbHeight / WorldHeightMetres;
    
    // Setup the Box2d world
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){4.0f, -10.0f};
    b2WorldId worldId = b2CreateWorld(&worldDef);
    

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
    

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){WorldWidthMetres/2, 80.0f};
    // bodyDef.rotation = b2MakeRot(0.7853982f);
    bodyDef.angularVelocity = -3.14f;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Polygon dynamicBox = b2MakeBox(SquareLengthMetres/2.0, SquareLengthMetres/2.0);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;
    shapeDef.restitution = 1.0f;
    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
    float timeStep = 1.0f / 60.0f;
    int subStepCount = 4;
    char formatted_string[100];
    // Loop forever
    while(1) {

        WPAD_ScanPads();  // Scan the Wiimotes

        // If [HOME] was pressed on the first Wiimote, break out of the loop
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)  break;

        // Draw code
        b2World_Step(worldId, timeStep, subStepCount);
        b2Vec2 position = b2Body_GetPosition(bodyId);
        b2Rot rotation = b2Body_GetRotation(bodyId);
        sprintf(formatted_string, "%4.2f %4.2f %4.2f\n", position.x, position.y, b2Rot_GetAngle(rotation));
        GRRLIB_FillScreen(0xFFFFFFFF);
        GRRLIB_Printf(10, 10, text_font1, 0X000000FF, 1, formatted_string);
        GRRLIB_Printf(10, 50, text_font1, 0X000000FF, 1, "WIDTH SCALE: %i", rmode->fbWidth);
        GRRLIB_Printf(10, 100, text_font1, 0X000000FF, 1, "HEIGTH SCALE: %i", rmode->efbHeight);
        // GRRLIB_Rectangle(
        //     (position.x-BoxHalfWidthMetres) * WorldToWindowWidthScale,
        //     (position.y-BoxHalfHeightMetres) * WorldToWindowHeightScale,
        //     BoxHalfWidthMetres * 2 * WorldToWindowWidthScale,
        //     BoxHalfHeightMetres * 2 * WorldToWindowHeightScale,
        //     GRRLIB_RED,
        //     1
        // );
        DrawRotatedRectangle(
            position.x,
            position.y,
            SquareLengthMetres,
            SquareLengthMetres,
            b2Rot_GetAngle(rotation),
            GRRLIB_RED
        );

        DrawRotatedRectangle(
            b2Body_GetPosition(groundId).x,
            b2Body_GetPosition(groundId).y,
            SquareLengthMetres,
            SquareLengthMetres,
            0,
            GRRLIB_GREEN
        );

        GRRLIB_Render();  // Render the frame buffer to the TV
    }

    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}
