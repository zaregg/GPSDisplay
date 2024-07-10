SET(SOURCES ui_Screen1.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    ui_img_compass_final_png.c
    ui_img_indicator_png.c
    ui_img_line_1_png.c
    ui_img_gradient_png.c
    ui_img_group_2_png.c
    ui_img_group_3_png.c
    fonts/ui_font_Font30.c
    fonts/ui_font_Font20.c
    fonts/ui_font_Font50.c
    fonts/ui_font_Font100.c)

add_library(ui ${SOURCES})
