/*===========================================
                  box2d-wii
               - hello_box2d -

    Port of the hello box2d example to wii
    Modified to drop the box from higher,
    with 0.8 of restitution (will bounce)

    Font taken from GRRLIB bitmap_fx example
============================================*/
#include <stdlib.h>
#include <stdio.h>
#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <box2d/box2d.h>

#include "font1_png.h"

int main(int argc, char **argv) {

    // Set-up font
    GRRLIB_texImg *text_font1 = GRRLIB_LoadTexture(font1_png);
    GRRLIB_InitTileSet(text_font1, 32, 36, 32);

    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();

    // Initialise the Wiimotes
    WPAD_Init();

    // Setup the Box2d world
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){0.0f, -10.0f};
    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){0.0f, 15.0f};
    // bodyDef.rotation = b2MakeRot(0.7853982f);
    // bodyDef.angularVelocity = 3.14f;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;
    shapeDef.restitution = 0.8f;
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
        GRRLIB_Printf(10, 50, text_font1, 0X000000FF, 1, formatted_string);
        GRRLIB_Render();  // Render the frame buffer to the TV
    }

    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}
