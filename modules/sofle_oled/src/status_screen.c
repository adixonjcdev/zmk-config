#include <zephyr/kernel.h>
#include <lvgl.h>

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    /* Eliminar todos los estilos heredados/default del objeto raíz */
    lv_obj_remove_style_all(screen);

    /* Fondo negro */
    lv_obj_set_style_bg_color(screen, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);

    /* Crear únicamente una etiqueta */
    lv_obj_t *label = lv_label_create(screen);

    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_label_set_text(label, "SOFLE");

    lv_obj_center(label);

    return screen;
}