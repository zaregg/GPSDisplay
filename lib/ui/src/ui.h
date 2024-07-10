// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.1
// LVGL version: 8.3.11
// Project name: Boat

#ifndef _BOAT_UI_H
#define _BOAT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "ui_events.h"

// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_compass;
extern lv_obj_t * ui_Indicator;
extern lv_obj_t * ui_Container1;
extern lv_obj_t * ui_lbl_degree;
extern lv_obj_t * ui_lbl_direction;
extern lv_obj_t * ui_Divider01;
extern lv_obj_t * ui_Divider02;
extern lv_obj_t * ui_Gradient01;
extern lv_obj_t * ui_Container2;
extern lv_obj_t * ui_lbl_speed;
extern lv_obj_t * ui_lbl_unit;
extern lv_obj_t * ui____initial_actions0;


LV_IMG_DECLARE(ui_img_compass_final_png);    // assets/compass_final.png
LV_IMG_DECLARE(ui_img_indicator_png);    // assets/Indicator.png
LV_IMG_DECLARE(ui_img_line_1_png);    // assets/Line 1.png
LV_IMG_DECLARE(ui_img_gradient_png);    // assets/gradient.png
LV_IMG_DECLARE(ui_img_group_2_png);    // assets/Group 2.png
LV_IMG_DECLARE(ui_img_group_3_png);    // assets/Group 3.png



LV_FONT_DECLARE(ui_font_Font30);
LV_FONT_DECLARE(ui_font_Font20);
LV_FONT_DECLARE(ui_font_Font50);
LV_FONT_DECLARE(ui_font_Font100);


void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif