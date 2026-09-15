#include <zephyr/kernel.h>
#include <lvgl.h>

#include <zmk/display/widgets/layer_status.h>

/* Widget oficial de ZMK para la capa activa */
static struct zmk_widget_layer_status layer_widget;

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    /*
     * SSD1306 monocromático:
     * en nuestro display la polaridad efectiva está invertida.
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
     * Título
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

    lv_label_set_text(title, "SOFLE");

    lv_obj_align(
        title,
        LV_ALIGN_TOP_LEFT,
        2,
        0
    );

    /*
     * Widget oficial de capa activa.
     */
    zmk_widget_layer_status_init(
        &layer_widget,
        screen
    );

    lv_obj_t *layer_obj =
        zmk_widget_layer_status_obj(&layer_widget);

    /*
     * Lo posicionamos en la parte inferior derecha.
     *
     * IMPORTANTE:
     * No intentamos modificar todavía los hijos internos
     * del widget. Primero queremos ver exactamente cuánto
     * espacio ocupa por sí mismo.
     */
    lv_obj_align(
        layer_obj,
        LV_ALIGN_BOTTOM_RIGHT,
        -2,
        -1
    );

    return screen;
}