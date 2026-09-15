#include <zephyr/kernel.h>
#include <lvgl.h>

#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>

#include "logo.h"


#define HID_NUM_LOCK    BIT(0)
#define HID_CAPS_LOCK   BIT(1)
#define HID_SCROLL_LOCK BIT(2)


static lv_obj_t *caps_label;
static lv_obj_t *num_label;
static lv_obj_t *scroll_label;


/*
 * Guardamos el último estado recibido desde el central.
 */
static zmk_hid_indicators_t current_indicators;


/*
 * ============================================================
 * ACTUALIZAR OLED
 * ============================================================
 */

static void update_indicators(void) {
    if (
        caps_label == NULL ||
        num_label == NULL ||
        scroll_label == NULL
    ) {
        return;
    }

    lv_label_set_text(
        caps_label,
        (current_indicators & HID_CAPS_LOCK)
            ? "CAPS"
            : ""
    );

    lv_label_set_text(
        num_label,
        (current_indicators & HID_NUM_LOCK)
            ? "NUM"
            : ""
    );

    lv_label_set_text(
        scroll_label,
        (current_indicators & HID_SCROLL_LOCK)
            ? "SCRL"
            : ""
    );
}


/*
 * ============================================================
 * EVENTO HID
 *
 * El central recibe el estado desde Windows y ZMK lo envía
 * al peripheral.
 *
 * El peripheral genera zmk_hid_indicators_changed.
 * ============================================================
 */

static int oled_indicators_listener(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *event =
        as_zmk_hid_indicators_changed(eh);

    if (event == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    current_indicators = event->indicators;

    update_indicators();

    return ZMK_EV_EVENT_BUBBLE;
}


ZMK_LISTENER(
    sofle_oled_indicators,
    oled_indicators_listener
);

ZMK_SUBSCRIPTION(
    sofle_oled_indicators,
    zmk_hid_indicators_changed
);


/*
 * ============================================================
 * ESTILO
 * ============================================================
 */

static void style_indicator(lv_obj_t *label) {
    lv_obj_remove_style_all(label);

    /*
     * En nuestro SSD1306:
     *
     * BLACK lógico = píxel físicamente encendido.
     */

    lv_obj_set_style_text_color(
        label,
        lv_color_black(),
        LV_PART_MAIN
    );

    lv_obj_set_style_text_opa(
        label,
        LV_OPA_COVER,
        LV_PART_MAIN
    );

    lv_obj_set_style_text_font(
        label,
        &lv_font_montserrat_10,
        LV_PART_MAIN
    );
}


/*
 * ============================================================
 * OLED DERECHO
 * ============================================================
 */

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);

    /*
     * Fondo
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
     * ========================================================
     * LOGO
     *
     * Volvemos a dejarlo CENTRADO.
     * ========================================================
     */

    lv_obj_t *logo = lv_image_create(screen);

    lv_image_set_src(
        logo,
        &adx_logo
    );

    lv_obj_center(logo);


    /*
     * CAPS
     * Superior izquierda
     */

    caps_label = lv_label_create(screen);

    style_indicator(caps_label);

    lv_obj_align(
        caps_label,
        LV_ALIGN_TOP_LEFT,
        0,
        0
    );


    /*
     * NUM
     * Superior derecha
     */

    num_label = lv_label_create(screen);

    style_indicator(num_label);

    lv_obj_align(
        num_label,
        LV_ALIGN_TOP_RIGHT,
        0,
        0
    );


    /*
     * SCROLL
     * Inferior derecha
     */

    scroll_label = lv_label_create(screen);

    style_indicator(scroll_label);

    lv_obj_align(
        scroll_label,
        LV_ALIGN_BOTTOM_RIGHT,
        0,
        0
    );


    /*
     * Inicialmente todos apagados.
     *
     * En cuanto el central nos envíe el estado,
     * el listener actualizará la pantalla.
     */

    current_indicators = 0;

    update_indicators();

    return screen;
}