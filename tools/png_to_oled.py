from PIL import Image

INPUT = "logo_pixel.png"
OUTPUT = "modules/sofle_oled/src/logo.h"

MAX_WIDTH = 48
MAX_HEIGHT = 30


# ============================================================
# CARGAR Y RECORTAR
# ============================================================

img = Image.open(INPUT).convert("RGBA")

pixels = img.load()

xs = []
ys = []

for y in range(img.height):
    for x in range(img.width):
        r, g, b, a = pixels[x, y]

        if a > 10 and min(r, g, b) < 240:
            xs.append(x)
            ys.append(y)

if not xs:
    raise RuntimeError("No se encontró contenido visible")

bbox = (
    min(xs),
    min(ys),
    max(xs) + 1,
    max(ys) + 1,
)

img = img.crop(bbox)


# ============================================================
# ESCALAR
# ============================================================

img.thumbnail(
    (MAX_WIDTH, MAX_HEIGHT),
    Image.Resampling.LANCZOS
)

width, height = img.size

print(f"Logo final: {width}x{height}")


# ============================================================
# CONVERTIR A I1
#
# LVGL I1 necesita:
#
# 2 colores de paleta = 8 bytes
# +
# bitmap 1-bit
#
# Cada fila ocupa ceil(width / 8) bytes.
# ============================================================

stride = (width + 7) // 8

bitmap = bytearray(stride * height)

for y in range(height):
    for x in range(width):

        r, g, b, a = img.getpixel((x, y))

        visible = (
            a > 30 and
            min(r, g, b) < 245
        )

        if visible:
            byte_index = y * stride + (x // 8)
            bit_index = 7 - (x % 8)

            bitmap[byte_index] |= (1 << bit_index)


# ============================================================
# PALETA I1
#
# Color 0: transparente
# Color 1: negro lógico
#
# En nuestro SSD1306 negro lógico termina siendo píxel blanco
# físicamente encendido.
#
# LVGL almacena la paleta como ARGB8888.
# ============================================================

palette = bytearray([
    # índice 0 -> transparente
    0x00, 0x00, 0x00, 0x00,

    # índice 1 -> negro completamente opaco
    0x00, 0x00, 0x00, 0xFF,
])

data = palette + bitmap


# ============================================================
# GENERAR HEADER
# ============================================================

with open(OUTPUT, "w", encoding="utf-8") as f:

    f.write("#pragma once\n\n")
    f.write("#include <lvgl.h>\n\n")

    f.write(
        "static const uint8_t adx_logo_map[] = {\n"
    )

    for i in range(0, len(data), 16):

        chunk = data[i:i + 16]

        f.write("    ")

        f.write(
            ", ".join(
                f"0x{value:02X}"
                for value in chunk
            )
        )

        f.write(",\n")

    f.write("};\n\n")

    f.write(
        "static const lv_image_dsc_t adx_logo = {\n"
    )

    f.write("    .header = {\n")
    f.write("        .magic = LV_IMAGE_HEADER_MAGIC,\n")
    f.write("        .cf = LV_COLOR_FORMAT_I1,\n")
    f.write("        .flags = 0,\n")
    f.write(f"        .w = {width},\n")
    f.write(f"        .h = {height},\n")
    f.write(f"        .stride = {stride},\n")
    f.write("        .reserved_2 = 0,\n")
    f.write("    },\n")

    f.write(
        "    .data_size = sizeof(adx_logo_map),\n"
    )

    f.write(
        "    .data = adx_logo_map,\n"
    )

    f.write(
        "    .reserved = NULL,\n"
    )

    f.write("};\n")