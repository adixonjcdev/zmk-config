#include <zephyr/kernel.h>
#include <lvgl.h>

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    /* Pantalla negra */
    lv_obj_set_style_bg_color(
        screen,
        lv_color_black(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_opa(
        screen,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    /* Texto */
    lv_obj_t *label = lv_label_create(screen);

    lv_label_set_text(label, "SOFLE");

    lv_obj_set_style_text_color(
        label,
        lv_color_white(),
        LV_PART_MAIN
    );

    lv_obj_set_style_text_font(
        label,
        &lv_font_montserrat_14,
        LV_PART_MAIN
    );

    lv_obj_center(label);

    return screen;
}