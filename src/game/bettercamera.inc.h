#include "sm64.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "game/print.h"
#include "engine/math_util.h"
#include "game/segment2.h"
#include "game/save_file.h"
#include "bettercamera.h"
#include "engine/surface_collision.h"
#include "pc/configfile.h"
#include "pc/controller/controller_mouse.h"
#include "pc/lua/utils/smlua_camera_utils.h"

#if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR) 
//quick and dirty fix for some older MinGW.org mingwrt
#else
#include <stdio.h>
#endif

#define NEW_CAM_BOUNDING_BOX_RAYS 4
#define NEW_CAM_BOUNDING_BOX_HRADIUS 250
#define NEW_CAM_BOUNDING_BOX_VRADIUS 100

/**
Quick explanation of the camera modes

NC_MODE_NORMAL: Standard mode, allows dualaxial movement and free control of the camera.
NC_MODE_FIXED: Disables control of camera, and the actual position of the camera doesn't update.
NC_MODE_2D: Disables horizontal control of the camera and locks Mario's direction to the X axis. NYI though.
NC_MODE_8D: 8 directional movement. Similar to standard, except the camera direction snaps to 8 directions.
NC_MODE_FIXED_NOMOVE: Disables control and movement of the camera.
NC_MODE_NOTURN: Disables horizontal and vertical control of the camera.

Coop only uses NC_MODE_NORMAL and NC_MODE_SLIDE
**/

//!A bunch of developer intended options, to cover every base, really.
//#define NEWCAM_DEBUG //Some print values for puppycam. Not useful anymore, but never hurts to keep em around.
//#define nosound //If for some reason you hate the concept of audio, you can disable it.
//#define noaccel //Disables smooth movement of the camera with the C buttons.

//!Hardcoded camera angle stuff. They're essentially area boxes that when Mario is inside, will trigger some view changes.
///Don't touch this btw, unless you know what you're doing, this has to be above for religious reasons.
UNUSED struct newcam_hardpos {
    u8 newcam_hard_levelID;
    u8 newcam_hard_areaID;
    u8 newcam_hard_permaswap;
    u16 newcam_hard_modeset;
    s16 newcam_hard_X1;
    s16 newcam_hard_Y1;
    s16 newcam_hard_Z1;
    s16 newcam_hard_X2;
    s16 newcam_hard_Y2;
    s16 newcam_hard_Z2;
    s16 newcam_hard_camX;
    s16 newcam_hard_camY;
    s16 newcam_hard_camZ;
    s16 newcam_hard_lookX;
    s16 newcam_hard_lookY;
    s16 newcam_hard_lookZ;
};

///This is the bit that defines where the angles happen. They're basically environment boxes that dictate camera behaviour.
//Permaswap is a boolean that simply determines wether or not when the camera changes at this point it stays changed. 0 means it resets when you leave, and 1 means it stays changed.
//The camera position fields accept "32767" as an ignore flag.
UNUSED struct newcam_hardpos newcam_fixedcam[] = {
    {
        /*Level ID*/ 16,/*Area ID*/ 1,/*Permaswap*/ 0,/*Mode*/ NC_MODE_FIXED_NOMOVE, //Standard params.
        /*X begin*/ -540,/*Y begin*/ 800,/*Z begin*/ -3500, //Where the activation box begins
        /*X end*/ 540,/*Y end*/ 2000,/*Z end*/ -1500, //Where the activation box ends.
        /*Cam X*/ 0,/*Cam Y*/ 1500,/*Cam Z*/ -1000, //The position the camera gets placed for NC_MODE_FIXED and NC_MODE_FIXED_NOMOVE
        /*Look X*/ 0,/*Look Y*/ 800,/*Look Z*/ -2500 //The position the camera looks at for NC_MODE_FIXED_NOMOVE
    },
};


#ifdef noaccel
u8 accel = 255;
#else
u8 accel = 10;
#endif // noaccel

s16 newcam_yaw; //Z axis rotation
f32 newcam_yaw_accel;
s16 newcam_tilt = 1500; //Y axis rotation
f32 newcam_tilt_accel;
u16 newcam_distance = 750; //The distance the camera stays from the player
u16 newcam_distance_target = 750; //The distance the player camera tries to reach.
f32 newcam_pos_target[3]; //The position the camera is basing calculations off. *usually* Mario.
f32 newcam_pos[3]; //Position the camera is in the world
f32 newcam_lookat[3]; //Position the camera is looking at
f32 newcam_framessincec[2];
f32 newcam_extheight = 125;
u8 newcam_centering = 0; // The flag that depicts wether the camera's goin gto try centering.
s16 newcam_yaw_target; // The yaw value the camera tries to set itself to when the centre flag is active. Is set to Mario's face angle.
f32 newcam_turnwait; // The amount of time to wait after landing before allowing the camera to turn again
f32 newcam_pan_x;
f32 newcam_pan_z;
u8 newcam_cstick_down = 0; //Just a value that triggers true when the player 2 stick is moved in 8 direction move to prevent holding it down.
u8 newcam_target;
s32 newcam_sintimer = 0;
s16 newcam_coldist;
u8 newcam_xlu = 255;
s8 newcam_stick2[2];

s16 newcam_sensitivityX; //How quick the camera works.
s16 newcam_sensitivityY;
s16 newcam_invertX; //Reverses movement of the camera axis.
s16 newcam_invertY;
s16 newcam_panlevel; //How much the camera sticks out a bit in the direction you're looking.
s16 newcam_aggression ; //How much the camera tries to centre itself to Mario's facing and movement.
s16 newcam_degrade = 1;
s16 newcam_analogue = 0; //Wether to accept inputs from a player 2 joystick, and then disables C button input.
s16 newcam_distance_values[] = {750,1250,2000};
u8 newcam_distance_index = 0;
u8 newcam_active = 0; // basically the thing that governs if newcam is on.
u8 newcam_mouse = 0;
u16 newcam_mode;
u16 newcam_intendedmode = 0; // which camera mode the camera's going to try to be in when not forced into another.
u16 newcam_modeflags;

s16 newcam_saved_mode = -1;
s16 newcam_saved_defmode = -1;

u8 newcam_use_dpad = FALSE;
u8 newcam_has_collision = TRUE;
u8 newcam_direction_locked = FALSE;
u8 newcam_l_centering = TRUE;

extern bool gDjuiInMainMenu;

///This is called at every level initialisation.
void newcam_init(struct Camera *c, u8 isSoftReset) {
    newcam_tilt = 1500;
    newcam_yaw = -c->yaw+0x4000; //Mario and the camera's yaw have this offset between them.
    newcam_mode = NC_MODE_NORMAL;
    ///This here will dictate what modes the camera will start in at the beginning of a level. Below are some examples.
    if (gCameraUseCourseSpecificSettings && !isSoftReset) {
        switch (gCurrLevelNum) {
            case LEVEL_BITDW: newcam_yaw = 0x4000; /*newcam_mode = NC_MODE_8D;*/ newcam_tilt = 4000; break;
            case LEVEL_BITFS: newcam_yaw = 0x4000; /*newcam_mode = NC_MODE_8D;*/ newcam_tilt = 4000; break;
            case LEVEL_BITS: newcam_yaw = 0x4000; /*newcam_mode = NC_MODE_8D;*/ newcam_tilt = 4000; break;
            case LEVEL_WF: newcam_yaw = 0x4000; newcam_tilt = 2000; break;
            case LEVEL_RR: newcam_yaw = 0x6000; newcam_tilt = 2000; break;
            case LEVEL_CCM: if (gCurrAreaIndex == 1) {newcam_yaw = -0x4000; newcam_tilt = 2000; } else newcam_mode = NC_MODE_SLIDE; break;
            case LEVEL_WDW: newcam_yaw = 0x2000; newcam_tilt = 3000; break;
            case LEVEL_PSS: newcam_mode = NC_MODE_SLIDE; break;
            case LEVEL_TTM: if (gCurrAreaIndex == 2) newcam_mode = NC_MODE_SLIDE; break;
        }
    }

    // clear these out when entering a new level to prevent "camera mode buffering"
    newcam_saved_defmode = -1;
    newcam_saved_mode = -1;

    // this will be set in init_settings() if enabled
    newcam_active = 0;

    newcam_distance = newcam_distance_target;
    newcam_intendedmode = newcam_mode;
    newcam_modeflags = newcam_mode;
}

static s16 newcam_clamp(s16 value, s16 min, s16 max) {
    if (value >= max)
        return max;
    else if (value <= min)
        return min;
    else
        return value;
}

void newcam_toggle(bool enabled) {
    // force-disable if a demo is being played
    if (gCurrDemoInput)
        enabled = false;

    if (enabled && !newcam_active) {
        newcam_active = 1;
        newcam_saved_mode = gLakituState.mode;
        newcam_saved_defmode = gLakituState.defMode;
        gLakituState.mode = CAMERA_MODE_NEWCAM;
        gLakituState.defMode = CAMERA_MODE_NEWCAM;
    } else if (!enabled && newcam_active) {
        if (newcam_saved_mode != -1) {
            gLakituState.defMode = newcam_saved_defmode;
            gLakituState.mode = newcam_saved_mode;
            newcam_saved_defmode = -1;
            newcam_saved_mode = -1;
        }
        newcam_active = 0;
    }
}

///These are the default settings for Puppycam. You may change them to change how they'll be set for first timers.
void newcam_init_settings(void) {
    newcam_sensitivityX     = newcam_clamp(camera_config_get_x_sensitivity(), 1, 100) * 5;
    newcam_sensitivityY     = newcam_clamp(camera_config_get_y_sensitivity(), 1, 100) * 5;
    newcam_aggression       = newcam_clamp(camera_config_get_aggression(), 0, 100);
    newcam_panlevel         = newcam_clamp(camera_config_get_pan_level(), 0, 100);
    newcam_invertX          = (s16)camera_config_is_x_inverted();
    newcam_invertY          = (s16)camera_config_is_y_inverted();
    newcam_mouse            = (u8)camera_config_is_mouse_look_enabled();
    newcam_analogue         = (s16)camera_config_is_analog_cam_enabled();
    newcam_degrade          = (f32)camera_config_get_deceleration();
    newcam_use_dpad         = (u8)camera_config_is_dpad_enabled();
    newcam_has_collision    = (u8)camera_config_is_collision_enabled();
    newcam_l_centering      = (u8)camera_config_get_centering();

    // setup main menu camera
    if (gDjuiInMainMenu) { newcam_tilt = 5; }

    newcam_toggle(camera_config_is_free_cam_enabled() || gDjuiInMainMenu);
}

void newcam_init_settings_override(bool override) {
    newcam_sensitivityX     = newcam_clamp(camera_config_get_x_sensitivity(), 1, 100) * 5;
    newcam_sensitivityY     = newcam_clamp(camera_config_get_y_sensitivity(), 1, 100) * 5;
    newcam_aggression       = newcam_clamp(camera_config_get_aggression(), 0, 100);
    newcam_panlevel         = newcam_clamp(camera_config_get_pan_level(), 0, 100);
    newcam_invertX          = (s16)camera_config_is_x_inverted();
    newcam_invertY          = (s16)camera_config_is_y_inverted();
    newcam_mouse            = (u8)camera_config_is_mouse_look_enabled();
    newcam_analogue         = (s16)camera_config_is_analog_cam_enabled();
    newcam_degrade          = (f32)camera_config_get_deceleration();
    newcam_use_dpad         = (u8)camera_config_is_dpad_enabled();
    newcam_has_collision    = (u8)camera_config_is_collision_enabled();
    newcam_l_centering      = (u8)camera_config_get_centering();

    // setup main menu camera
    if (gDjuiInMainMenu) { newcam_tilt = 5; }

    newcam_toggle(override);
}

/** Mathematic calculations. This stuffs so basic even *I* understand it lol
Basically, it just returns a position based on angle */
static s16 lengthdir_x(f32 length, s16 dir) {
    return (s16) (length * coss(dir));
}
static s16 lengthdir_y(f32 length, s16 dir) {
    return (s16) (length * sins(dir));
}

void newcam_diagnostics(void) {
    print_text_fmt_int(32,192,"Lv %d",gCurrLevelNum);
    print_text_fmt_int(32,176,"Area %d",gCurrAreaIndex);
    print_text_fmt_int(32,160,"X %d", gMarioStates[0].pos[0]);
    print_text_fmt_int(32,144,"Y %d", gMarioStates[0].pos[1]);
    print_text_fmt_int(32,128,"Z %d", gMarioStates[0].pos[2]);
    print_text_fmt_int(32,112,"FLAGS %d",newcam_modeflags);
    print_text_fmt_int(180,112,"INTM %d",newcam_intendedmode);
    print_text_fmt_int(32,96,"TILT UP %d",newcam_tilt_accel);
    print_text_fmt_int(32,80,"YAW UP %d",newcam_yaw_accel);
    print_text_fmt_int(32,64,"YAW %d",newcam_yaw);
    print_text_fmt_int(32,48,"TILT  %d",newcam_tilt);
    print_text_fmt_int(32,32,"DISTANCE %d",newcam_distance);
}

static s16 newcam_adjust_value(f32 var, f32 val, f32 max) {
    if (val > 0.0f) {
        var += val;
        if (var > max)
            var = max;
    } else if (val < 0.0f) {
        var += val;
        if (var < max)
            var = max;
    }

    return var;
}

static f32 newcam_approach_float(f32 var, f32 val, f32 inc) {
    if (var < val)
        return min(var + inc, val);
    else
        return max(var - inc, val);
}

static s16 newcam_approach_s16(s16 var, s16 val, s16 inc) {
    if (var < val)
        return max(var + inc, val);
    else
        return min(var - inc, val);
}

static int ivrt(u8 axis) {
    if (axis == 0) {
        if (newcam_invertX == 0)
            return -1;
        else
            return 1;
    } else {
        if (newcam_invertY == 0)
            return 1;
        else
            return -1;
    }
}

static void newcam_rotate_button(void) {
    f32 intendedXMag;
    f32 intendedYMag;

    // Unused section for coop
    if ((newcam_modeflags & NC_FLAG_8D || newcam_modeflags & NC_FLAG_4D) && newcam_modeflags & NC_FLAG_XTURN) {
        //8 directional camera rotation input for buttons.
        if ((gPlayer1Controller->buttonPressed & L_CBUTTONS) && newcam_analogue == 0) {
#ifndef nosound
            play_sound(SOUND_MENU_CAMERA_TURN, gGlobalSoundSource);
#endif
            if (newcam_modeflags & NC_FLAG_8D)
                newcam_yaw_target = newcam_yaw_target+(ivrt(0)*0x2000);
            else
                newcam_yaw_target = newcam_yaw_target+(ivrt(0)*0x4000);
            newcam_centering = 1;
        } else if ((gPlayer1Controller->buttonPressed & R_CBUTTONS) && newcam_analogue == 0) {
#ifndef nosound
            play_sound(SOUND_MENU_CAMERA_TURN, gGlobalSoundSource);
#endif
            if (newcam_modeflags & NC_FLAG_8D)
                newcam_yaw_target = newcam_yaw_target-(ivrt(0)*0x2000);
            else
                newcam_yaw_target = newcam_yaw_target-(ivrt(0)*0x4000);
            newcam_centering = 1;
        }
    // Used section for coop
    } else if (newcam_modeflags & NC_FLAG_XTURN) {
        //Standard camera movement
        // Buzz if the camera can't move due to being locked
        if (gPlayer1Controller->buttonPressed & (L_CBUTTONS | R_CBUTTONS) && newcam_direction_locked) {
#ifndef nosound
            play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
#endif
        }
        if ((gPlayer1Controller->buttonDown & L_CBUTTONS) && newcam_analogue == 0) {
            newcam_yaw_accel = newcam_adjust_value(newcam_yaw_accel, -accel, -100);
        } else if ((gPlayer1Controller->buttonDown & R_CBUTTONS) && newcam_analogue == 0) {
            newcam_yaw_accel = newcam_adjust_value(newcam_yaw_accel, accel, 100);
        } else if (!newcam_analogue) {
#ifdef noaccel
            newcam_yaw_accel = 0;
#else
            newcam_yaw_accel -= (newcam_yaw_accel*((f32)newcam_degrade/100));
#endif
        }
    }

    if (gPlayer1Controller->buttonDown & U_CBUTTONS && newcam_modeflags & NC_FLAG_YTURN && newcam_analogue == 0) {
        newcam_tilt_accel = newcam_adjust_value(newcam_tilt_accel, accel, 100);
    } else if (gPlayer1Controller->buttonDown & D_CBUTTONS && newcam_modeflags & NC_FLAG_YTURN && newcam_analogue == 0) {
        newcam_tilt_accel = newcam_adjust_value(newcam_tilt_accel, -accel, -100);
    } else if (!newcam_analogue) {
#ifdef noaccel
        newcam_tilt_accel = 0;
#else
        newcam_tilt_accel -= (newcam_tilt_accel*((f32)newcam_degrade/100));
#endif
    }

    newcam_framessincec[0] ++;
    newcam_framessincec[1] ++;
    if (!newcam_direction_locked) {
        if ((gPlayer1Controller->buttonPressed & L_CBUTTONS) && newcam_modeflags & NC_FLAG_XTURN && !(newcam_modeflags & NC_FLAG_8D) && newcam_analogue == 0) {
            if (newcam_framessincec[0] < 6) {
                newcam_yaw_target = newcam_yaw+(ivrt(0)*0x3000);
                newcam_centering = 1;
#ifndef nosound
                play_sound(SOUND_MENU_CAMERA_TURN, gGlobalSoundSource);
#endif
            }
            newcam_framessincec[0] = 0;
        }
        if ((gPlayer1Controller->buttonPressed & R_CBUTTONS) && newcam_modeflags & NC_FLAG_XTURN && !(newcam_modeflags & NC_FLAG_8D) && newcam_analogue == 0) {
            if (newcam_framessincec[1] < 6) {
                newcam_yaw_target = newcam_yaw-(ivrt(0)*0x3000);
                newcam_centering = 1;
#ifndef nosound
                play_sound(SOUND_MENU_CAMERA_TURN, gGlobalSoundSource);
#endif
            }
            newcam_framessincec[1] = 0;
        }
    }



    //There's not much point in keeping this behind a check, but it wouldn't hurt, just incase any 2player shenanigans ever happen, it makes it easy to disable.
    if (newcam_analogue == 1) {
        //The joystick values cap at 80, so divide by 8 to get the same net result at maximum turn as the button
        intendedXMag = newcam_stick2[0]*1.25;
        intendedYMag = newcam_stick2[1]*1.25;

        if (ABS(newcam_stick2[0]) > 20 && newcam_modeflags & NC_FLAG_XTURN) {
            if (newcam_modeflags & NC_FLAG_8D) {
                if (newcam_cstick_down == 0) {
                    newcam_cstick_down = 1;
                    newcam_centering = 1;
#ifndef nosound
                    play_sound(SOUND_MENU_CAMERA_TURN, gGlobalSoundSource);
#endif
                    if (newcam_stick2[0] > 20) {
                        if (newcam_modeflags & NC_FLAG_8D)
                            newcam_yaw_target = newcam_yaw_target+(ivrt(0)*0x2000);
                        else
                            newcam_yaw_target = newcam_yaw_target+(ivrt(0)*0x4000);
                    } else {
                        if (newcam_modeflags & NC_FLAG_8D)
                            newcam_yaw_target = newcam_yaw_target-(ivrt(0)*0x2000);
                        else
                            newcam_yaw_target = newcam_yaw_target-(ivrt(0)*0x4000);
                    }
                }
            } else {
                newcam_yaw_accel = newcam_adjust_value(newcam_yaw_accel,newcam_stick2[0]*0.125, intendedXMag);
            }
        } else if (newcam_analogue) {
            newcam_cstick_down = 0;
            newcam_yaw_accel -= (newcam_yaw_accel*((f32)newcam_degrade/100));
        }

        if (ABS(newcam_stick2[1]) > 20 && newcam_modeflags & NC_FLAG_YTURN)
            newcam_tilt_accel = newcam_adjust_value(newcam_tilt_accel, newcam_stick2[1]*0.125, intendedYMag);
        else if (newcam_analogue)
            newcam_tilt_accel -= (newcam_tilt_accel*((f32)newcam_degrade/100));
    }

    if ((newcam_mouse == 1) && !gDjuiInMainMenu && !gDjuiChatBoxFocus && !gDjuiConsoleFocus) {
        if (!newcam_use_dpad || !newcam_direction_locked) {
            newcam_yaw += (f32)ivrt(0) * (f32)mouse_x * 16.0f * ((f32)newcam_sensitivityX / 250.0f);
        }
        newcam_tilt += (f32)ivrt(1) * (f32)mouse_y * 16.0f * ((f32)newcam_sensitivityY / 250.0f);
    }

    // Dpad behaviors
    if (newcam_use_dpad) {
        // Make dpad up head to the nearest cardinal direction
        if (gPlayer1Controller->buttonDown & U_JPAD) {
            newcam_yaw = snap_to_45_degrees(newcam_yaw);
            newcam_yaw_accel = snap_to_45_degrees(newcam_yaw_accel);
        }
        // Make dpad left/right increment 45 degrees
        else if (gPlayer1Controller->buttonPressed & L_JPAD) {
            newcam_yaw += ivrt(0)*DEGREES(45);
#ifndef nosound
            play_sound(SOUND_MENU_CAMERA_TURN, gGlobalSoundSource);
#endif
        }
        else if (gPlayer1Controller->buttonPressed & R_JPAD) {
            newcam_yaw -= ivrt(0)*DEGREES(45);
#ifndef nosound
            play_sound(SOUND_MENU_CAMERA_TURN, gGlobalSoundSource);
#endif
        }
        // Make dpad down lock the current camera direction
        else if (gPlayer1Controller->buttonPressed & D_JPAD) {
            newcam_direction_locked = !newcam_direction_locked;
        }
    }

    if (newcam_use_dpad && newcam_direction_locked) {
        newcam_yaw_accel = 0;
    }
}

static void newcam_zoom_button(void) {
    if (gDjuiInMainMenu) {
        newcam_distance = 750;
        newcam_distance_target = 750;
        return;
    }
    
    //Smoothly move the camera to the new spot.
    if (newcam_distance > newcam_distance_target) {
        newcam_distance -= 250;
        if (newcam_distance < newcam_distance_target)
            newcam_distance = newcam_distance_target;
    }
    if (newcam_distance < newcam_distance_target) {
        newcam_distance += 250;
        if (newcam_distance > newcam_distance_target)
            newcam_distance = newcam_distance_target;
    }

    if (newcam_l_centering && (gPlayer1Controller->buttonDown & L_TRIG) && (newcam_modeflags & NC_FLAG_ZOOM)) {
        //When you press L, set the flag for centering the camera. Afterwards, start setting the yaw to the Player's yaw at the time.
        newcam_yaw_target = -gMarioStates[0].faceAngle[1]-0x4000;
        newcam_centering = 1;
    } else if (gPlayer1Controller->buttonPressed & R_TRIG && newcam_modeflags & NC_FLAG_XTURN) {
        //Each time the player presses R, but NOT L the camera zooms out more, until it hits the limit and resets back to close view.
#ifndef nosound
        play_sound(SOUND_MENU_CLICK_CHANGE_VIEW, gGlobalSoundSource);
#endif

        if (newcam_distance_target == newcam_distance_values[0]) {
            newcam_distance_target = newcam_distance_values[1];
            newcam_distance_index = 1;
        } else if (newcam_distance_target == newcam_distance_values[1]) {
            newcam_distance_target = newcam_distance_values[2];
            newcam_distance_index = 2;
        } else {
            newcam_distance_target = newcam_distance_values[0];
            newcam_distance_index = 0;
        }
    }

    if (newcam_centering && newcam_modeflags & NC_FLAG_XTURN) {
        newcam_yaw = approach_s16_symmetric(newcam_yaw,newcam_yaw_target,0x800);
        if (newcam_yaw == newcam_yaw_target)
            newcam_centering = 0;
    } else {
        newcam_yaw_target = newcam_yaw;
    }
}

static void newcam_update_values(void) {
    //For tilt, this just limits it so it doesn't go further than 90 degrees either way. 90 degrees is actually 16384, but can sometimes lead to issues, so I just leave it shy of 90.
    u8 waterflag = 0;
    u8 centering = 0;
    u8 ycentering = 1;

    if (newcam_modeflags & NC_FLAG_XTURN)
        newcam_yaw -= ((newcam_yaw_accel*(newcam_sensitivityX/10))*ivrt(0));
    if (((newcam_tilt <= 0x3000) && (newcam_tilt >= -0x3000)) && newcam_modeflags & NC_FLAG_YTURN)
        newcam_tilt += ((newcam_tilt_accel*ivrt(1))*(newcam_sensitivityY/10));

    if (newcam_tilt > 0x3000)
        newcam_tilt = 0x3000;
    if (newcam_tilt < -0x3000 && gMarioStates[0].pos[1] - gMarioStates[0].floorHeight > 20)
        newcam_tilt = -0x3000;
    if (newcam_tilt < -0x3000 && gMarioStates[0].pos[1] - gMarioStates[0].floorHeight < 20)
        newcam_tilt = -0x3000;

    if (newcam_turnwait > 0 && gMarioStates[0].vel[1] == 0) {
        newcam_turnwait -= 1;
        if (newcam_turnwait < 0)
            newcam_turnwait = 0;
    } else {
        if (gMarioStates[0].intendedMag > 0 && gMarioStates[0].vel[1] == 0 && newcam_modeflags & NC_FLAG_XTURN && !(newcam_modeflags & NC_FLAG_8D) && !(newcam_modeflags & NC_FLAG_4D))
            newcam_yaw = (approach_s16_symmetric(newcam_yaw,-gMarioStates[0].faceAngle[1]-0x4000,((newcam_aggression*(ABS(gPlayer1Controller->rawStickX/10)))*(gMarioStates[0].forwardVel/32))));
        else
            newcam_turnwait = 10;
    }

    if (newcam_modeflags & NC_FLAG_SLIDECORRECT) {
        switch (gMarioStates[0].action) {
            case ACT_BUTT_SLIDE: if (gMarioStates[0].forwardVel > 8) centering = 1; ycentering = 0; break;
            case ACT_STOMACH_SLIDE: if (gMarioStates[0].forwardVel > 8) centering = 1; ycentering = 0; break;
            case ACT_HOLD_BUTT_SLIDE: if (gMarioStates[0].forwardVel > 8) centering = 1; ycentering = 0; break;
            case ACT_HOLD_STOMACH_SLIDE: if (gMarioStates[0].forwardVel > 8) centering = 1; ycentering = 0; break;
        }
    }

    if ((gMarioStates[0].action & ACT_FLAG_FLYING) == ACT_FLAG_FLYING) {
        centering = 1;
    }

    static u32 sLastAction = 0;
    static u8 sForceCentering = 10;
    if (sLastAction != gMarioStates[0].action) {
        sLastAction = gMarioStates[0].action;
        sForceCentering = 1;
        switch (gMarioStates[0].action) {
            case ACT_SHOT_FROM_CANNON:
                newcam_yaw = -gMarioStates[0].faceAngle[1]-0x4000;
                break;
        }
    }

    if (centering) {
        if (fabs(newcam_yaw_accel) > 32 || fabs(newcam_tilt_accel) > 32) {
            sForceCentering = 0;
        }
        if (sForceCentering) { waterflag = 1; }
    }

    if (gMarioStates[0].action & ACT_FLAG_SWIMMING) {
        if (gMarioStates[0].forwardVel > 2)
        waterflag = 1;
    }

    if (waterflag && newcam_modeflags & NC_FLAG_XTURN) {
        newcam_yaw = (approach_s16_symmetric(newcam_yaw,-gMarioStates[0].faceAngle[1]-0x4000,(gMarioStates[0].forwardVel*128)));
        if ((signed)gMarioStates[0].forwardVel > 1 && ycentering)
            newcam_tilt = (approach_s16_symmetric(newcam_tilt,(-gMarioStates[0].faceAngle[0]*0.8)+3000,(gMarioStates[0].forwardVel*32)));
        else
            newcam_tilt = (approach_s16_symmetric(newcam_tilt,3000,32));
    }
}

static void newcam_collision(void) {
    // check if we can see player
    Vec3f up = { 0, 1, 0 };
    Vec3f mainRay = {
        newcam_pos[0]-newcam_lookat[0],
        newcam_pos[1]-newcam_lookat[1],
        newcam_pos[2]-newcam_lookat[2],
    };
    Vec3f sideway;
    vec3f_normalize(mainRay);
    vec3f_cross(sideway, mainRay, up);

    f32 checkWidth = 75;
    f32 checkHeight = 90;

    bool allhit = true;
    for (f32 x = -1; x <= 1; x++) {
        for (f32 y = 0; y <= 1; y++) {
            if (fabs(x) + fabs(y) != 1) { continue; }
            Vec3f offset = {
                sideway[0] * x * checkWidth,
                -checkHeight / 2 + y * checkHeight,
                sideway[2] * x * checkWidth,
            };

            if (x != 0) {
                struct Surface* surf;
                Vec3f hitpos;
                Vec3f move = {
                    offset[0] * 1.2f,
                    offset[1],
                    offset[2] * 1.2f,
                };
                find_surface_on_ray(newcam_pos_target, move, &surf, hitpos, 3.0f);
                vec3f_copy(offset, hitpos);
                vec3f_sub(offset, newcam_pos_target);
                if (surf) {
                    offset[0] *= 0;
                    offset[2] *= 0;
                }
            }

            Vec3f camray = {
                newcam_pos[0] - newcam_lookat[0] - offset[0],
                newcam_pos[1] - newcam_lookat[1] - offset[1],
                newcam_pos[2] - newcam_lookat[2] - offset[2],
            };

            Vec3f camorig = {
                newcam_pos_target[0] + offset[0],
                newcam_pos_target[1] + offset[1],
                newcam_pos_target[2] + offset[2],
            };

            struct Surface* surf;
            Vec3f hitpos;
            find_surface_on_ray(camorig, camray, &surf, hitpos, 3.0f);

            if (surf == NULL) {
                allhit = false;
            }
        }
    }

    Vec3f camdir = {
        newcam_pos[0]-newcam_lookat[0],
        newcam_pos[1]-newcam_lookat[1],
        newcam_pos[2]-newcam_lookat[2],
    };

    if (allhit) {
        struct Surface *surf = NULL;
        Vec3f hitpos;

        find_surface_on_ray(newcam_lookat, camdir, &surf, hitpos, 3.0f);

        if (surf) {
            // offset the hit pos by the hit normal
            Vec3f offset = { 0 };
            offset[0] = surf->normal.x;
            offset[1] = surf->normal.y;
            offset[2] = surf->normal.z;
            vec3f_mul(offset, 5.0f);
            vec3f_add(hitpos, offset);

            newcam_pos[0] = hitpos[0];
            newcam_pos[1] = hitpos[1];
            newcam_pos[2] = hitpos[2];
            newcam_pan_x = 0;
            newcam_pan_z = 0;
        }

        newcam_coldist = vec3f_dist(newcam_pos_target, hitpos);

    } else {
        newcam_pos[0] = newcam_lookat[0] + camdir[0];
        newcam_pos[1] = newcam_lookat[1] + camdir[1];
        newcam_pos[2] = newcam_lookat[2] + camdir[2];
        newcam_coldist = vec3f_length(camdir);
    }
}

static void newcam_set_pan(void) {
    if (gDjuiInMainMenu) {
        newcam_pan_x = 0;
        newcam_pan_z = 0;
        return;
    }

    //Apply panning values based on Mario's direction.
    if (gMarioStates[0].action != ACT_HOLDING_BOWSER && gMarioStates[0].action != ACT_SLEEPING && gMarioStates[0].action != ACT_START_SLEEPING) {
        approach_f32_asymptotic_bool(&newcam_pan_x, lengthdir_x((160*newcam_panlevel)/100, -gMarioStates[0].faceAngle[1]-0x4000), 0.05);
        approach_f32_asymptotic_bool(&newcam_pan_z, lengthdir_y((160*newcam_panlevel)/100, -gMarioStates[0].faceAngle[1]-0x4000), 0.05);
    } else {
        approach_f32_asymptotic_bool(&newcam_pan_x, 0, 0.05);
        approach_f32_asymptotic_bool(&newcam_pan_z, 0, 0.05);
    }

    newcam_pan_x = newcam_pan_x*(min(newcam_distance/newcam_distance_target,1));
    newcam_pan_z = newcam_pan_z*(min(newcam_distance/newcam_distance_target,1));
}

static void newcam_level_bounds(void) {
    u8 hasMinY = FALSE;
    f32 minY = 0;
    get_area_minimum_y(&hasMinY, &minY);
    if (!hasMinY) { return; }
    newcam_pos[1] = MAX(newcam_pos[1], minY);
}

static void newcam_position_cam(void) {
    f32 floorY = 0;
    f32 floorY2 = 0;
    s16 shakeX;
    s16 shakeY;

    if (!(gMarioStates[0].action & ACT_FLAG_SWIMMING) && newcam_modeflags & NC_FLAG_FOCUSY && newcam_modeflags & NC_FLAG_POSY)
        calc_y_to_curr_floor(&floorY, 1.f, 200.f, &floorY2, 0.9f, 200.f);

    newcam_update_values();
    shakeX = gLakituState.shakeMagnitude[1];
    shakeY = gLakituState.shakeMagnitude[0];
    //Fetch Mario's current position. Not hardcoded just for the sake of flexibility, though this specific bit is temp, because it won't always want to be focusing on Mario.
    newcam_pos_target[0] = gMarioStates[0].pos[0];
    newcam_pos_target[1] = gMarioStates[0].pos[1]+newcam_extheight;
    newcam_pos_target[2] = gMarioStates[0].pos[2];
    //These will set the position of the camera to where Mario is supposed to be, minus adjustments for where the camera should be, on top of.
    if (newcam_modeflags & NC_FLAG_POSX)
        newcam_pos[0] = newcam_pos_target[0]+lengthdir_x(lengthdir_x(newcam_distance,newcam_tilt+shakeX),newcam_yaw+shakeY);
    if (newcam_modeflags & NC_FLAG_POSZ)
        newcam_pos[2] = newcam_pos_target[2]+lengthdir_y(lengthdir_x(newcam_distance,newcam_tilt+shakeX),newcam_yaw+shakeY);
    if (newcam_modeflags & NC_FLAG_POSY)
        newcam_pos[1] = newcam_pos_target[1]+lengthdir_y(newcam_distance,newcam_tilt+gLakituState.shakeMagnitude[0])+floorY;
    if ((newcam_modeflags & NC_FLAG_FOCUSX) && (newcam_modeflags & NC_FLAG_FOCUSY) && (newcam_modeflags & NC_FLAG_FOCUSZ))
        newcam_set_pan();
    //Set where the camera wants to be looking at. This is almost always the place it's based off, too.
    if (newcam_modeflags & NC_FLAG_FOCUSX)
        newcam_lookat[0] = newcam_pos_target[0]-newcam_pan_x;
    if (newcam_modeflags & NC_FLAG_FOCUSY)
        newcam_lookat[1] = newcam_pos_target[1]+floorY2;
    if (newcam_modeflags & NC_FLAG_FOCUSZ)
        newcam_lookat[2] = newcam_pos_target[2]-newcam_pan_z;

    newcam_level_bounds();
    if (newcam_has_collision && newcam_modeflags & NC_FLAG_COLLISION) {
        newcam_collision();
    }

}

//Nested if's baybeeeee
UNUSED static void newcam_find_fixed(void) {
    u8 i = 0;
    newcam_mode = newcam_intendedmode;
    newcam_modeflags = newcam_mode;
    for (i = 0; i < sizeof(newcam_fixedcam) / sizeof(struct newcam_hardpos); i++) {
        if (newcam_fixedcam[i].newcam_hard_levelID == gCurrLevelNum && newcam_fixedcam[i].newcam_hard_areaID == gCurrAreaIndex) {
            if ((newcam_pos_target[0] > newcam_fixedcam[i].newcam_hard_X1)
            &&  (newcam_pos_target[0] < newcam_fixedcam[i].newcam_hard_X2)
            &&  (newcam_pos_target[1] > newcam_fixedcam[i].newcam_hard_Y1)
            &&  (newcam_pos_target[1] < newcam_fixedcam[i].newcam_hard_Y2)
            &&  (newcam_pos_target[2] > newcam_fixedcam[i].newcam_hard_Z1)
            &&  (newcam_pos_target[2] < newcam_fixedcam[i].newcam_hard_Z2)) {
                if (newcam_fixedcam[i].newcam_hard_permaswap)
                    newcam_intendedmode = newcam_fixedcam[i].newcam_hard_modeset;
                newcam_mode = newcam_fixedcam[i].newcam_hard_modeset;
                newcam_modeflags = newcam_mode;

                if (newcam_fixedcam[i].newcam_hard_camX != 32767 && !(newcam_modeflags & NC_FLAG_POSX))
                    newcam_pos[0] = newcam_fixedcam[i].newcam_hard_camX;
                if (newcam_fixedcam[i].newcam_hard_camY != 32767 && !(newcam_modeflags & NC_FLAG_POSY))
                    newcam_pos[1] = newcam_fixedcam[i].newcam_hard_camY;
                if (newcam_fixedcam[i].newcam_hard_camZ != 32767 && !(newcam_modeflags & NC_FLAG_POSZ))
                    newcam_pos[2] = newcam_fixedcam[i].newcam_hard_camZ;

                if (newcam_fixedcam[i].newcam_hard_lookX != 32767 && !(newcam_modeflags & NC_FLAG_FOCUSX))
                    newcam_lookat[0] = newcam_fixedcam[i].newcam_hard_lookX;
                if (newcam_fixedcam[i].newcam_hard_lookY != 32767 && !(newcam_modeflags & NC_FLAG_FOCUSY))
                    newcam_lookat[1] = newcam_fixedcam[i].newcam_hard_lookY;
                if (newcam_fixedcam[i].newcam_hard_lookZ != 32767 && !(newcam_modeflags & NC_FLAG_FOCUSZ))
                    newcam_lookat[2] = newcam_fixedcam[i].newcam_hard_lookZ;

                newcam_yaw = atan2s(newcam_pos[0]-newcam_pos_target[0],newcam_pos[2]-newcam_pos_target[2]);
            }
        }
    }
}

static void newcam_apply_values(struct Camera *c) {
    c->pos[0] = newcam_pos[0];
    c->pos[1] = newcam_pos[1];
    c->pos[2] = newcam_pos[2];

    c->focus[0] = newcam_lookat[0];
    c->focus[1] = newcam_lookat[1];
    c->focus[2] = newcam_lookat[2];

    gLakituState.pos[0] = newcam_pos[0];
    gLakituState.pos[1] = newcam_pos[1];
    gLakituState.pos[2] = newcam_pos[2];

    gLakituState.focus[0] = newcam_lookat[0];
    gLakituState.focus[1] = newcam_lookat[1];
    gLakituState.focus[2] = newcam_lookat[2];

    c->yaw = -newcam_yaw+0x4000;
    gLakituState.yaw = -newcam_yaw+0x4000;

    //Adds support for wing mario tower
    if (gMarioStates[0].floor != NULL) {
        if (gMarioStates[0].floor->type == SURFACE_LOOK_UP_WARP) {
            if (save_file_get_total_star_count(gCurrSaveFileNum - 1, 0, 0x18) >= gLevelValues.wingCapLookUpReq) {
                if (newcam_tilt < -8000 && gMarioStates[0].forwardVel == 0 && sCurrPlayMode != PLAY_MODE_PAUSED) {
                    level_trigger_warp(gMarioState, 1);
                }
            }
        }
    }
}

//If puppycam gets too close to its target, start fading it out so you don't see the inside of it.
void newcam_fade_target_closeup(void) {
    if (newcam_coldist <= 250 && (newcam_coldist-150)*2.55f < 255) {
        if ((newcam_coldist-150)*2.55f > 0)
            newcam_xlu = (newcam_coldist-150)*2.55f;
        else
            newcam_xlu = 0;
    } else {
        newcam_xlu = 255;
    }
}

//The ingame cutscene system is such a spaghetti mess I actually have to resort to something as stupid as this to cover every base.
void newcam_apply_outside_values(struct Camera *c, u8 bit) {
    if (bit)
        newcam_yaw = -gMarioStates[0].faceAngle[1]-0x4000;
    else
        newcam_yaw = -c->yaw+0x4000;
}

static void newcam_stick_input(void) {
    newcam_stick2[0] = gPlayer1Controller->extStickX;
    newcam_stick2[1] = gPlayer1Controller->extStickY;
}

//Main loop.
void newcam_loop(struct Camera *c) {
    if (sCurrPlayMode != PLAY_MODE_PAUSED) {
        newcam_stick_input();
        newcam_rotate_button();
        newcam_zoom_button();
    } else {
        newcam_yaw_accel *= 0.6f;
        newcam_tilt_accel *= 0.6f;
    }
    newcam_position_cam();
    //newcam_find_fixed();
    if (gMarioObject)
        newcam_apply_values(c);
    newcam_fade_target_closeup();

    //Just some visual information on the values of the camera. utilises ifdef because it's better at runtime.
#ifdef NEWCAM_DEBUG
    newcam_diagnostics();
#endif // NEWCAM_DEBUG
}
