from PIL import Image

INPUT = "logo_pixel.png"
OUTPUT = "modules/sofle_oled/src/logo.h"

# Dejamos aire alrededor del logo dentro del OLED 128x32.
MAX_WIDTH = 40
MAX_HEIGHT = 26

# Umbral de cobertura.
# Un píxel debe tener al menos esta opacidad para encenderse.
ALPHA_THRESHOLD = 128


# ============================================================
# CARGAR IMAGEN
# ============================================================

img = Image.open(INPUT).convert("RGBA")


# ============================================================
# ENCONTRAR EL CONTENIDO REAL
#
# Aquí NO importa si el logo es naranja, blanco, negro, etc.
# Solamente importa que el píxel no sea transparente.
# ============================================================

alpha = img.getchannel("A")

bbox = alpha.getbbox()

if bbox is None:
    raise RuntimeError(
        "No se encontró contenido visible en la imagen"
    )

img = img.crop(bbox)

original_width, original_height = img.size

print(
    f"Original recortado: "
    f"{original_width}x{original_height}"
)


# ============================================================
# CALCULAR TAMAÑO FINAL
#
# Conservamos estrictamente la relación de aspecto.
# ============================================================

scale = min(
    MAX_WIDTH / original_width,
    MAX_HEIGHT / original_height
)

width = max(
    1,
    round(original_width * scale)
)

height = max(
    1,
    round(original_height * scale)
)

print(
    f"Logo final: {width}x{height}"
)


# ============================================================
# ESCALAR
#
# LANCZOS genera buenos bordes al reducir.
#
# Los píxeles semitransparentes que produce LANCZOS se
# conservarán temporalmente y después decidiremos cuáles
# sobreviven al convertir a 1 bit.
# ============================================================

img = img.resize(
    (width, height),
    Image.Resampling.LANCZOS
)


# ============================================================
# BITMAP I1
# ============================================================

stride = (width + 7) // 8

bitmap = bytearray(
    stride * height
)


# ============================================================
# CONVERSIÓN A 1 BIT
#
# Usamos exclusivamente ALPHA.
#
# Esto es importante porque el isotipo tiene:
#
#   naranja -> logo
#   blanco  -> logo
#
# Ambos deben convertirse exactamente igual.
#
# La diferencia con nuestra versión anterior es que ya no
# aceptamos prácticamente cualquier píxel semitransparente.
#
# Con threshold 128:
#
# alpha 255 -> ON
# alpha 200 -> ON
# alpha 128 -> ON
# alpha  80 -> OFF
# alpha  20 -> OFF
#
# Esto limpia bastante las diagonales después del escalado.
# ============================================================

for y in range(height):

    for x in range(width):

        _, _, _, a = img.getpixel(
            (x, y)
        )

        if a >= ALPHA_THRESHOLD:

            byte_index = (
                y * stride
                + (x // 8)
            )

            bit_index = (
                7 - (x % 8)
            )

            bitmap[byte_index] |= (
                1 << bit_index
            )


# ============================================================
# PALETA LVGL I1
#
# 0 = transparente
# 1 = negro lógico opaco
#
# En nuestro SSD1306 negro lógico corresponde al píxel
# físicamente encendido.
# ============================================================

palette = bytearray([
    # índice 0: transparente
    0x00, 0x00, 0x00, 0x00,

    # índice 1: negro lógico / opaco
    0x00, 0x00, 0x00, 0xFF,
])


data = palette + bitmap


# ============================================================
# GENERAR logo.h
# ============================================================

with open(
    OUTPUT,
    "w",
    encoding="utf-8"
) as f:

    f.write("#pragma once\n\n")
    f.write("#include <lvgl.h>\n\n")

    f.write(
        "static const uint8_t "
        "adx_logo_map[] = {\n"
    )

    for i in range(
        0,
        len(data),
        16
    ):

        chunk = data[i:i + 16]

        values = ", ".join(
            f"0x{value:02X}"
            for value in chunk
        )

        f.write(
            f"    {values},\n"
        )

    f.write("};\n\n")


    # ========================================================
    # DESCRIPTOR LVGL 9
    # ========================================================

    f.write(
        "static const lv_image_dsc_t "
        "adx_logo = {\n"
    )

    f.write(
        "    .header = {\n"
    )

    f.write(
        "        .magic = "
        "LV_IMAGE_HEADER_MAGIC,\n"
    )

    f.write(
        "        .cf = "
        "LV_COLOR_FORMAT_I1,\n"
    )

    f.write(
        "        .flags = 0,\n"
    )

    f.write(
        f"        .w = {width},\n"
    )

    f.write(
        f"        .h = {height},\n"
    )

    f.write(
        f"        .stride = {stride},\n"
    )

    f.write(
        "        .reserved_2 = 0,\n"
    )

    f.write(
        "    },\n"
    )

    f.write(
        "    .data_size = "
        "sizeof(adx_logo_map),\n"
    )

    f.write(
        "    .data = adx_logo_map,\n"
    )

    f.write(
        "    .reserved = NULL,\n"
    )

    f.write(
        "};\n"
    )


# ============================================================
# INFORMACIÓN FINAL
# ============================================================

print()
print("Conversión terminada")
print("--------------------")

print(
    f"Tamaño: {width}x{height}"
)

print(
    f"Stride: {stride} bytes"
)

print(
    f"Bitmap: {len(bitmap)} bytes"
)

print(
    f"Datos LVGL: {len(data)} bytes"
)

print(
    f"Archivo: {OUTPUT}"
)