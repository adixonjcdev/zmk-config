#include <zephyr/kernel.h>
#include <lvgl.h>
#include <stdio.h>

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/wpm.h>

static lv_obj_t *layer_label;
static lv_obj_t *wpm_value_label;


/*
 * ============================================================
 * LAYER
 * ============================================================
 */

/*
 * Obtiene la capa activa más alta y actualiza el texto.
 */
static void update_layer_label(void) {
    if (layer_label == NULL) {
        return;
    }

    zmk_keymap_layer_index_t layer =
        zmk_keymap_highest_layer_active();

    const char *name =
        zmk_keymap_layer_name(layer);

    if (name == NULL || name[0] == '\0') {
        name = "BASE";
    }

    lv_label_set_text(layer_label, name);
}


/*
 * Escucha los cambios de layer de ZMK.
 */
static int oled_layer_listener(const zmk_event_t *eh) {
    if (as_zmk_layer_state_changed(eh) != NULL) {
        update_layer_label();
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(
    sofle_oled_layer,
    oled_layer_listener
);

ZMK_SUBSCRIPTION(
    sofle_oled_layer,
    zmk_layer_state_changed
);


/*
 * ============================================================
 * WPM
 * ============================================================
 */

/*
 * Obtiene el WPM actual y actualiza el número mostrado.
 */
static void update_wpm_label(void) {
    if (wpm_value_label == NULL) {
        return;
    }

    char buffer[8];

    uint8_t wpm = zmk_wpm_get_state();

    snprintf(
        buffer,
        sizeof(buffer),
        "%u",
        wpm
    );

    lv_label_set_text(
        wpm_value_label,
        buffer
    );
}


/*
 * Escucha los cambios de WPM de ZMK.
 */
static int oled_wpm_listener(const zmk_event_t *eh) {
    if (as_zmk_wpm_state_changed(eh) != NULL) {
        update_wpm_label();
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(
    sofle_oled_wpm,
    oled_wpm_listener
);

ZMK_SUBSCRIPTION(
    sofle_oled_wpm,
    zmk_wpm_state_changed
);


/*
 * ============================================================
 * OLED
 * ============================================================
 */

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    /*
     * En nuestro SSD1306:
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
     * --------------------------------------------------------
     * SOFLE
     * Arriba a la izquierda.
     * --------------------------------------------------------
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
        &lv_font_montserrat_12,
        LV_PART_MAIN
    );

    lv_label_set_text(
        title,
        "SOFLE"
    );

    lv_obj_align(
        title,
        LV_ALIGN_TOP_LEFT,
        2,
        0
    );


    /*
     * --------------------------------------------------------
     * LAYER
     * Arriba a la derecha.
     * --------------------------------------------------------
     */

    layer_label = lv_label_create(screen);

    lv_obj_remove_style_all(layer_label);

    lv_obj_set_style_text_color(
        layer_label,
        lv_color_black(),
        LV_PART_MAIN
    );

    lv_obj_set_style_text_opa(
        layer_label,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    lv_obj_set_style_text_font(
        layer_label,
        &lv_font_montserrat_12,
        LV_PART_MAIN
    );

    update_layer_label();

    lv_obj_align(
        layer_label,
        LV_ALIGN_TOP_RIGHT,
        0,
        0
    );


    /*
     * --------------------------------------------------------
     * WPM
     * Texto fijo abajo a la izquierda.
     * --------------------------------------------------------
     */

    lv_obj_t *wpm_text = lv_label_create(screen);

    lv_obj_remove_style_all(wpm_text);

    lv_obj_set_style_text_color(
        wpm_text,
        lv_color_black(),
        LV_PART_MAIN
    );

    lv_obj_set_style_text_opa(
        wpm_text,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    lv_obj_set_style_text_font(
        wpm_text,
        &lv_font_montserrat_12,
        LV_PART_MAIN
    );

    lv_label_set_text(
        wpm_text,
        "WPM"
    );

    lv_obj_align(
        wpm_text,
        LV_ALIGN_BOTTOM_LEFT,
        2,
        0
    );


    /*
     * --------------------------------------------------------
     * VALOR WPM
     * Número dinámico abajo a la derecha.
     * --------------------------------------------------------
     */

    wpm_value_label = lv_label_create(screen);

    lv_obj_remove_style_all(wpm_value_label);

    lv_obj_set_style_text_color(
        wpm_value_label,
        lv_color_black(),
        LV_PART_MAIN
    );

    lv_obj_set_style_text_opa(
        wpm_value_label,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    lv_obj_set_style_text_font(
        wpm_value_label,
        &lv_font_montserrat_12,
        LV_PART_MAIN
    );

    update_wpm_label();

    lv_obj_align(
        wpm_value_label,
        LV_ALIGN_BOTTOM_RIGHT,
        0,
        0
    );


    return screen;
}