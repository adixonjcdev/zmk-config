#include <zephyr/kernel.h>
#include <lvgl.h>

#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>
#include <zmk/hid_indicators_types.h>

#include "logo.h"


/*
 * Bits estándar de los indicadores HID.
 */
#define HID_NUM_LOCK    BIT(0)
#define HID_CAPS_LOCK   BIT(1)
#define HID_SCROLL_LOCK BIT(2)


static lv_obj_t *caps_label;
static lv_obj_t *num_label;
static lv_obj_t *scroll_label;


/*
 * ============================================================
 * ACTUALIZAR INDICADORES
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

    zmk_hid_indicators_t indicators =
        zmk_hid_indicators_get_current_profile();


    /*
     * Si está activo mostramos el texto.
     * Si está apagado mostramos "".
     */

    lv_label_set_text(
        caps_label,
        (indicators & HID_CAPS_LOCK)
            ? "CAPS"
            : ""
    );

    lv_label_set_text(
        num_label,
        (indicators & HID_NUM_LOCK)
            ? "NUM"
            : ""
    );

    lv_label_set_text(
        scroll_label,
        (indicators & HID_SCROLL_LOCK)
            ? "SCRL"
            : ""
    );
}


/*
 * ============================================================
 * EVENTO HID
 *
 * Windows/Linux/etc. informa al teclado cuando cambia uno
 * de los estados:
 *
 * CAPS
 * NUM
 * SCROLL
 * ============================================================
 */

static int oled_indicators_listener(
    const zmk_event_t *eh
) {

    if (
        as_zmk_hid_indicators_changed(eh)
        != NULL
    ) {
        update_indicators();
    }

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
 * ESTILO COMÚN DE LOS INDICADORES
 * ============================================================
 */

static void style_indicator(
    lv_obj_t *label
) {

    lv_obj_remove_style_all(label);

    /*
     * SSD1306:
     * BLACK lógico = píxel blanco físicamente encendido.
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
 * PANTALLA OLED DERECHA
 * ============================================================
 */

lv_obj_t *zmk_display_status_screen(void) {

    lv_obj_t *screen =
        lv_obj_create(NULL);


    /*
     * ========================================================
     * FONDO
     * ========================================================
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
     * LOGO ADX
     *
     * Lo dejamos EXACTAMENTE centrado.
     * ========================================================
     */

    lv_obj_t *logo =
        lv_image_create(screen);

    lv_image_set_src(
        logo,
        &adx_logo
    );

    lv_obj_center(logo);


    /*
     * ========================================================
     * CAPS LOCK
     *
     * Esquina superior izquierda.
     * ========================================================
     */

    caps_label =
        lv_label_create(screen);

    style_indicator(
        caps_label
    );

    lv_obj_align(
        caps_label,
        LV_ALIGN_TOP_LEFT,
        0,
        0
    );


    /*
     * ========================================================
     * NUM LOCK
     *
     * Esquina superior derecha.
     * ========================================================
     */

    num_label =
        lv_label_create(screen);

    style_indicator(
        num_label
    );

    lv_obj_align(
        num_label,
        LV_ALIGN_TOP_RIGHT,
        0,
        0
    );


    /*
     * ========================================================
     * SCROLL LOCK
     *
     * Esquina inferior derecha.
     * ========================================================
     */

    scroll_label =
        lv_label_create(screen);

    style_indicator(
        scroll_label
    );

    lv_obj_align(
        scroll_label,
        LV_ALIGN_BOTTOM_RIGHT,
        0,
        0
    );


    /*
     * Leer el estado inicial.
     */

    update_indicators();


    return screen;
}