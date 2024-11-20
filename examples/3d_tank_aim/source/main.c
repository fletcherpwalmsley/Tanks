
/*===========================================
                  Tanks?
            - 3d_tank_aim -

    Builds to 3d_tank_pan to make the tank point
        towards the wiimote cursor

============================================*/
#include <grrlib.h>

#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "font_png.h"
#include "crosshair_png.h"


// Function to calculate the dot product of two vectors
double dotProduct(double x1, double y1, double x2, double y2) {
    return x1 * x2 + y1 * y2;
}

// Function to calculate the magnitude of a vector
double magnitude(double x, double y) {
    return sqrt(x * x + y * y);
}

// Function to calculate the angle between two points in radians
double angleBetweenPoints(double x1, double y1, double x2, double y2) {
    // Calculate the dot product of vectors (x1, y1) and (x2, y2)
    double dot = dotProduct(x1, y1, x2, y2);

    // Calculate the magnitudes of the vectors
    double mag1 = magnitude(x1, y1);
    double mag2 = magnitude(x2, y2);

    // Compute the cosine of the angle
    double cosTheta = dot / (mag1 * mag2);

    // Clamp the value of cosTheta to avoid precision errors leading to invalid results
    if (cosTheta > 1.0) cosTheta = 1.0;
    if (cosTheta < -1.0) cosTheta = -1.0;

    // Calculate the angle in radians
    double theta = acos(cosTheta);

    return theta;
}

// Function to convert radians to degrees
double radiansToDegrees(double radians) {
    return radians * 180.0 / M_PI;
}


int main(void){
    GRRLIB_Init();
    WPAD_Init();

    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(font_png);
    GRRLIB_InitTileSet(tex_font, 16, 16, 32);


    // 2d wiimote items
    GRRLIB_texImg* crosshair  = GRRLIB_LoadTexturePNG(crosshair_png);
    GRRLIB_SetMidHandle(crosshair, true);

    // Initialise the Wiimotes
    const u16 WinW = rmode->fbWidth;
    const u16 WinH = rmode->efbHeight;
    ir_t P1Mote;
    WPAD_SetIdleTimeout(60 * 10);
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    

    GRRLIB_Settings.antialias = true;

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    f32 view_angle = 0.0f;
    f32 turret_angle = 30.0f;

    while(1) {
        GRRLIB_Camera3dSettings(0.0f,0.0f,5.0f, 1,0,0, 0,0,0);
        GRRLIB_2dMode();
        WPAD_ScanPads();
        WPAD_SetVRes(WPAD_CHAN_0, WinW, WinH);
        WPAD_IR(WPAD_CHAN_0, &P1Mote);  
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);

        // Wii Remote IR Viewport correction
        int P1MX = P1Mote.sx - 150;
        int P1MY = P1Mote.sy - 150;

        turret_angle+=0.4;
        if (turret_angle>360) {
            turret_angle=0.0;
        }

        GRRLIB_3dMode(0.1,1000,45,0,1);
        GRRLIB_SetLightAmbient(0x111111FF);
        GRRLIB_SetLightDiff(0,(guVector){0.0f,3.0f,3.0f},20.0f,1.0f,0xFFFFFFFF);

        // Tracks
        GRRLIB_ObjectViewBegin();
        GRRLIB_ObjectViewScale(1.0f, 1.0f, 0.5f);
        GRRLIB_ObjectViewRotate(0.0f,view_angle,0.0f);
        GRRLIB_ObjectViewEnd();
        GRRLIB_DrawCube(1.0f,1,0Xd9b77cFF);

        // Hull
        GRRLIB_ObjectViewBegin();
        GRRLIB_ObjectViewScale(0.7f, 1.1f, 0.501f);
        GRRLIB_ObjectViewRotate(0.0f,view_angle,0.0f);
        GRRLIB_ObjectViewEnd();
        GRRLIB_DrawCube(1.0f,1,0x5ba047FF);

        // Turret
        GRRLIB_ObjectViewBegin();
        GRRLIB_ObjectViewTrans(0.0f, 0.0f, 0.5);
        GRRLIB_ObjectViewScale(1.0f, 1.0f, 0.8f);
        GRRLIB_ObjectViewRotate(0.0f, view_angle, turret_angle);
        GRRLIB_ObjectViewEnd();
        GRRLIB_DrawCube(0.4f,1,0x5ba047FF);

        // Barrel
        GRRLIB_ObjectViewBegin();
        // GRRLIB_ObjectViewRotate(0.0f, 0.0f, turret_angle);
        GRRLIB_ObjectViewTrans(0.0f, 0.5f, 0.425);
        // GRRLIB_ObjectViewScale(1.0f, 1.0f, 0.8f);
        GRRLIB_ObjectViewRotate(0.0f, view_angle, turret_angle);
        GRRLIB_ObjectViewEnd();
        GRRLIB_DrawCylinder(0.09, 0.8, 15, 1, 0Xd9b77cFF);

        GRRLIB_2dMode();

        // Switch To 2D Mode to display text
        GRRLIB_2dMode();    
        // Calculate the angle between the points in radians
        // double angleRad = angleBetweenPoints(P1MX, P1MY, 320, 240);
        GRRLIB_DrawImg(P1MX, P1MY, crosshair, 0, 1, 1, RGBA(255, 255, 255, 255));
        float w1 = P1MX;
        float v1 = P1MX;
        float w2 = 320;
        float v2 = 240;

        double theta = atan2((w2 * v1 + w1 * v2), (w1 * v1 + w2 * v2));
        
        GRRLIB_Printf((640-(16*29))/2, 20, tex_font, 0xFFFFFFFF, 1, "TURRET ANGLE: %4.2f", turret_angle);
        GRRLIB_Printf((640-(16*29))/2, 40, tex_font, 0xFFFFFFFF, 1, "P1MX: %i", P1MX);
        GRRLIB_Printf((640-(16*29))/2, 60, tex_font, 0xFFFFFFFF, 1, "P1MY: %i", P1MY);
        GRRLIB_Printf((640-(16*29))/2, 80, tex_font, 0xFFFFFFFF, 1, "POINTER TO TURRET RAD: %4.2f", theta);
        GRRLIB_Printf((640-(16*29))/2, 100, tex_font, 0xFFFFFFFF, 1, "POINTER TO TURRET DEG: %4.2f", radiansToDegrees(theta));
        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
