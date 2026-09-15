#include <zephyr/kernel.h>
#include <lvgl.h>

#include "logo.h"

/*
 * Pantalla personalizada del OLED derecho.
 */
lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    /*
     * SSD1306:
     *
     * WHITE lógico = píxel apagado / fondo negro físico
     * BLACK lógico = píxel encendido / blanco físico
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
     * Creamos el logo píxel por píxel.
     *
     * El bitmap generado contiene:
     * 0xFF = píxel perteneciente al logo
     * 0x00 = píxel vacío
     */
    for (int y = 0; y < ADX_LOGO_HEIGHT; y++) {
        for (int x = 0; x < ADX_LOGO_WIDTH; x++) {

            int index = (y * ADX_LOGO_WIDTH) + x;

            if (adx_logo_map[index] == 0xFF) {

                lv_obj_t *pixel = lv_obj_create(screen);

                lv_obj_remove_style_all(pixel);

                lv_obj_set_size(
                    pixel,
                    1,
                    1
                );

                lv_obj_set_style_bg_color(
                    pixel,
                    lv_color_black(),
                    LV_PART_MAIN
                );

                lv_obj_set_style_bg_opa(
                    pixel,
                    LV_OPA_COVER,
                    LV_PART_MAIN
                );

                /*
                 * Centrar logo de 45x30 dentro de 128x32.
                 *
                 * X = (128 - 45) / 2 = 41
                 * Y = (32 - 30) / 2 = 1
                 */
                lv_obj_set_pos(
                    pixel,
                    41 + x,
                    1 + y
                );
            }
        }
    }

    return screen;
}