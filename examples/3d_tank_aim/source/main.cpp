
/*===========================================
                  Tanks?
            - 3d_tank_aim -

    Builds to 3d_tank_pan to make the tank point
        towards the wiimote cursor

============================================*/
#include <grrlib.h>

#include <stdlib.h>
#include <cmath>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "font_png.h"
#include "crosshair_png.h"

const int fiddle_angle = 30;

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

/**
 * Convert a point in the 2d view plane to the 2d game plane
 * This is a 2d operation as the rotation is constrand around an
 * axis
 * 
 * Will almost certainly be something in Grrlib or libogc to do this
 * work for me in the main game
 */
void viewToGamePlaneTranslate(const double& angle,
                              const int& viewX,
                              const int& viewY,
                              double& gameX,
                              double& gameY) {
    double angle_rad = angle * M_PI / 180.0;
    // gameX = cos(angle_rad)*viewX - sin(angle_rad)*viewY;
    gameX = viewX;
    // gameY = sin(angle_rad)*viewX + cos(angle_rad)*viewY;
    gameY = cos(angle_rad)*viewY;
}


// void viewToGamePlaneTranslate(const double& angle,
//                               const int& viewX,
//                               const int& viewY,
//                               double& gameX,
//                               double& gameY) {
//     (void) angle;
//     gameX = viewX;
//     gameY = viewY;
// }


double calculateAngle(double x, double y, double offset) {
    // Shift the point (x, y) relative to the new origin (320, 240)
    // const int viewMidX = 320;
    // const int viewMidY = 240;
    // double gameMidX;
    // double gameMidY;
    // viewToGamePlaneTranslate(fiddle_angle, viewMidX, viewMidY, gameMidX, gameMidY);

    y += (offset/4);
    double gameMidX = 320;
    double gameMidY = 205 - (offset/2);
    
    double shiftedX = x - gameMidX;
    double shiftedY = y - gameMidY;

    // Calculate the dot product of the y-axis vector (0, 1) and the shifted vector
    double dotProduct = shiftedY;

    // Calculate the magnitudes of the vectors
    double magnitudeV = sqrt(shiftedX * shiftedX + shiftedY * shiftedY);

    // Calculate the cosine of the angle
    double cosTheta = dotProduct / magnitudeV;

    // Calculate the angle in radians
    double angleRadians = acos(cosTheta);

    // Convert the angle to degrees
    double angleDegrees = angleRadians * (180.0 / M_PI);

    // Bottom, Right
    if ((x > gameMidX) && (y > gameMidY)) {
        return 90 + angleDegrees;
    }
    // Top, Right
    else if ((x > gameMidX) && (y < gameMidY)) {
        return angleDegrees + 90;
    }
    // Only need to translate axis for left side of the screen
    else {
        return 90 - angleDegrees;
    }
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
    f32 view_angle = 30.0f;
    f32 turret_angle = 0.0f;
    while(1) {
        GRRLIB_Camera3dSettings(0.0f,0.0f,5.0f, 1,0,0, 0,0,0);
        GRRLIB_2dMode();
        WPAD_ScanPads();
        WPAD_SetVRes(WPAD_CHAN_0, WinW, WinH);
        WPAD_IR(WPAD_CHAN_0, &P1Mote);  
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);

        // Wii Remote IR Viewport correction
        double P1MX = P1Mote.sx - 190;
        double P1MY = P1Mote.sy - 190;

        turret_angle+=0.4;
        if (turret_angle>360) {
            turret_angle=0.0;
        }

        // First, translate the points to the game plane
        double gameX = 0;
        double gameY = 0;
        double correctCordSysY = (480 - P1MY);
        viewToGamePlaneTranslate(fiddle_angle, P1MX, correctCordSysY, gameX, gameY);

        // double offset = correctCordSysY - gameY;
        double offset = 0;

        // Find the angle for the turret!
        double theta = calculateAngle(P1MX, P1MY, offset);
        // double theta = 0;

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
        GRRLIB_ObjectViewScale(0.75f, 1.1f, 0.501f);
        GRRLIB_ObjectViewRotate(0.0f,view_angle,0.0f);
        GRRLIB_ObjectViewEnd();
        GRRLIB_DrawCube(1.0f,1,0x5ba047FF);

        // Turret
        GRRLIB_ObjectViewBegin();
        GRRLIB_ObjectViewTrans(0.0f, 0.0f, 0.5);
        GRRLIB_ObjectViewScale(1.0f, 1.0f, 0.8f);
        GRRLIB_ObjectViewRotate(0.0f, view_angle, theta);
        GRRLIB_ObjectViewEnd();
        GRRLIB_DrawCube(0.4f,1,0x5ba047FF);

        // Barrel
        GRRLIB_ObjectViewBegin();
        // GRRLIB_ObjectViewRotate(0.0f, 0.0f, turret_angle);
        GRRLIB_ObjectViewTrans(0.0f, 0.5f, 0.425);
        // GRRLIB_ObjectViewScale(1.0f, 1.0f, 0.8f);
        GRRLIB_ObjectViewRotate(0.0f, view_angle, theta);
        GRRLIB_ObjectViewEnd();
        GRRLIB_DrawCylinder(0.1, 0.8, 15, 1, 0Xd9b77cFF);

        GRRLIB_2dMode();

        // Switch To 2D Mode to display text
        GRRLIB_2dMode();    
        // Calculate the angle between the points in radians
        // double angleRad = angleBetweenPoints(P1MX, P1MY, 320, 240);
        GRRLIB_DrawImg(P1MX, P1MY, crosshair, 0, 0.3, 0.3, RGBA(255, 255, 255, 255));
        GRRLIB_Line(320, 205, P1MX, P1MY, 0x00FF00FF);
        // GRRLIB_DrawImg(gameX, P1MY - offset, crosshair, 0, 1, 1, RGBA(255, 0, 0, 255));

        // GRRLIB_Printf((640-(16*29))/2, 20, tex_font, 0xFFFFFFFF, 1, "TURRET ANGLE: %4.2f", turret_angle);
        // GRRLIB_Printf((640-(16*29))/2, 40, tex_font, 0xFFFFFFFF, 1, "GAMEX: %4.2f", gameX);
        // GRRLIB_Printf((640-(16*29))/2, 60, tex_font, 0xFFFFFFFF, 1, "P1MX: %4.2f", P1MX);

        GRRLIB_Printf((640-(16*29))/2, 20, tex_font, 0xFFFFFFFF, 1, "P1MY: %4.2f", P1MY);
        GRRLIB_Printf((640-(16*29))/2, 40, tex_font, 0xFFFFFFFF, 1, "INVVIEWY: %4.2f", correctCordSysY);
        GRRLIB_Printf((640-(16*29))/2, 60, tex_font, 0xFFFFFFFF, 1, "GAMEY: %4.2f", gameY);
        GRRLIB_Printf((640-(16*29))/2, 80, tex_font, 0xFFFFFFFF, 1, "OFFSET: %4.2f", offset);

        GRRLIB_Printf((640-(16*29))/2, 140, tex_font, 0xFFFFFFFF, 1, "POINTER TO TURRET DEG: %4.2f", theta);
        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
