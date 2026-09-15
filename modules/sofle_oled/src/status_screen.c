#include <zephyr/kernel.h>
#include <lvgl.h>

#include <zmk/display/widgets/layer_status.h>

/* Widget oficial de ZMK */
static struct zmk_widget_layer_status layer_widget;

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    /*
     * IMPORTANTE:
     * En nuestro SSD1306 monocromático la polaridad
     * efectiva está invertida.
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
     * Título: SOFLE
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
        4,
        2
    );

    /*
     * Texto fijo: LAYER
     */
    lv_obj_t *layer_text = lv_label_create(screen);

    lv_obj_remove_style_all(layer_text);

    lv_obj_set_style_text_color(
        layer_text,
        lv_color_black(),
        LV_PART_MAIN
    );

    lv_obj_set_style_text_opa(
        layer_text,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    lv_obj_set_style_text_font(
        layer_text,
        &lv_font_montserrat_14,
        LV_PART_MAIN
    );

    lv_label_set_text(layer_text, "LAYER");

    lv_obj_align(
        layer_text,
        LV_ALIGN_BOTTOM_LEFT,
        4,
        -2
    );

    /*
     * Widget dinámico oficial de ZMK.
     *
     * Este es el que cambiará:
     * BASE -> LOWER -> RAISE -> ADJUST
     */
    zmk_widget_layer_status_init(
        &layer_widget,
        screen
    );

    lv_obj_t *layer_obj =
        zmk_widget_layer_status_obj(&layer_widget);

    /*
     * Quitamos cualquier estilo visual que no nos interese
     * y aplicamos nuestra polaridad.
     */
    lv_obj_set_style_text_color(
        layer_obj,
        lv_color_black(),
        LV_PART_MAIN
    );

    lv_obj_set_style_text_opa(
        layer_obj,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    lv_obj_set_style_text_font(
        layer_obj,
        &lv_font_montserrat_14,
        LV_PART_MAIN
    );

    lv_obj_align(
        layer_obj,
        LV_ALIGN_BOTTOM_RIGHT,
        -4,
        -2
    );

    return screen;
}