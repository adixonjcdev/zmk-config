#include <zephyr/kernel.h>
#include <lvgl.h>

#include "logo.h"


/*
 * Pantalla personalizada del OLED derecho.
 */
lv_obj_t *zmk_display_status_screen(void) {

    /*
     * ========================================================
     * SCREEN
     * ========================================================
     */

    lv_obj_t *screen = lv_obj_create(NULL);

    lv_obj_remove_style_all(screen);

    /*
     * SSD1306:
     *
     * WHITE lógico = píxel apagado
     * BLACK lógico = píxel encendido
     */

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
     * ========================================================
     * LOGO ADX
     * ========================================================
     */

    lv_obj_t *logo = lv_image_create(screen);

    lv_image_set_src(
        logo,
        &adx_logo
    );

    /*
     * Logo 45x30
     * OLED 128x32
     *
     * Lo centramos completamente.
     */

    lv_obj_center(logo);


    return screen;
}