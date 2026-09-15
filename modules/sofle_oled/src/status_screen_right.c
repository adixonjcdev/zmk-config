#include <zephyr/kernel.h>
#include <lvgl.h>

/*
 * Pantalla personalizada del OLED derecho.
 */
lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    /*
     * SSD1306:
     *
     * WHITE lógico = píxel apagado
     * BLACK lógico = píxel encendido
     */
    lv_obj_remove_style_all(screen);

    lv_obj_set_style_bg_color(
        screen,
        lv_color_white(),
        LV_PART_MAIN
    );

    lv_obj_set_style_bg_opa(
        screen,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    /*
     * Texto de prueba.
     */
    lv_obj_t *title = lv_label_create(screen);

    lv_obj_remove_style_all(title);

    lv_obj_set_style_text_color(
        title,
        lv_color_black(),
        LV_PART_MAIN
    );

    lv_obj_set_style_text_opa(
        title,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    lv_obj_set_style_text_font(
        title,
        &lv_font_montserrat_14,
        LV_PART_MAIN
    );

    lv_label_set_text(
        title,
        "SOFLE"
    );

    lv_obj_center(title);

    return screen;
}