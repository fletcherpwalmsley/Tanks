
/*===========================================
                  Tanks?
            - 3d_tank_pan -

    Pan the camera around a basic shape

============================================*/
#include <grrlib.h>

#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "font_png.h"

int main(void){
    GRRLIB_Init();
    WPAD_Init();

    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(font_png);
    GRRLIB_InitTileSet(tex_font, 16, 16, 32);

    GRRLIB_Settings.antialias = true;

    GRRLIB_SetBackgroundColour(0x00, 0x00, 0x00, 0xFF);
    f32 view_angle = 30.0f;
    f32 turret_angle = 30.0f;

    while(1) {
        GRRLIB_Camera3dSettings(0.0f,0.0f,5.0f, 1,0,0, 0,0,0);
        GRRLIB_2dMode();
        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);

        view_angle+=0.2;
        turret_angle+=0.4;
        if (turret_angle>360) {
            turret_angle=0.0;
        }

        if (view_angle>360) {
            view_angle=0.0;
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
        GRRLIB_Printf((640-(16*29))/2, 20, tex_font, 0xFFFFFFFF, 1, "TURRET ANGLE: %4.2f", turret_angle);
        GRRLIB_Printf((640-(16*29))/2, 40, tex_font, 0xFFFFFFFF, 1, "CAMERA ANGLE: %4.2f", view_angle);
        GRRLIB_Render();
    }
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);
}
