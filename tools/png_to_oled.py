from PIL import Image

INPUT = "logo_pixel.png"
OUTPUT = "modules/sofle_oled/src/logo.h"

MAX_WIDTH = 48
MAX_HEIGHT = 30


# ============================================================
# CARGAR IMAGEN
# ============================================================

img = Image.open(INPUT).convert("RGBA")


# ============================================================
# DETECTAR ÁREA VISIBLE
#
# IMPORTANTE:
# No comprobamos el color del píxel.
#
# El logo contiene elementos blancos y naranjas, por lo que
# usar RGB para determinar si un píxel pertenece al logo
# eliminaría las partes blancas.
#
# Aquí solamente importa el canal alpha:
#
# alpha > 10  -> pertenece al logo
# alpha <= 10 -> transparente / fondo
# ============================================================

pixels = img.load()

xs = []
ys = []

for y in range(img.height):
    for x in range(img.width):

        r, g, b, a = pixels[x, y]

        if a > 10:
            xs.append(x)
            ys.append(y)


if not xs:
    raise RuntimeError(
        "No se encontró contenido visible en la imagen"
    )


# ============================================================
# RECORTAR ESPACIO TRANSPARENTE
# ============================================================

bbox = (
    min(xs),
    min(ys),
    max(xs) + 1,
    max(ys) + 1,
)

img = img.crop(bbox)


# ============================================================
# ESCALAR
#
# Conservamos la proporción original del isotipo.
#
# Nunca será mayor que:
#
# 48 px de ancho
# 30 px de alto
# ============================================================

img.thumbnail(
    (MAX_WIDTH, MAX_HEIGHT),
    Image.Resampling.LANCZOS
)

width, height = img.size

print(f"Logo final: {width}x{height}")


# ============================================================
# CONVERTIR A BITMAP MONOCROMÁTICO I1
#
# Cada píxel necesita solamente 1 bit.
#
# Ejemplo:
#
# 45 px de ancho
#
# ceil(45 / 8) = 6 bytes por fila
#
# LVGL llama a esto "stride".
# ============================================================

stride = (width + 7) // 8

bitmap = bytearray(
    stride * height
)


# ============================================================
# GENERAR PIXELES
#
# IMPORTANTE:
#
# Ya NO comprobamos:
#
#     min(r, g, b) < 245
#
# porque eso eliminaba el blanco.
#
# Ahora:
#
# naranja     -> ON
# blanco      -> ON
# negro       -> ON
# cualquier
# otro color  -> ON
#
# transparente -> OFF
#
# Para el OLED monocromático todos los colores visibles
# terminan convertidos en píxeles encendidos.
# ============================================================

for y in range(height):
    for x in range(width):

        r, g, b, a = img.getpixel((x, y))

        visible = a > 30

        if visible:

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
# I1 utiliza dos colores:
#
# índice 0 -> transparente
# índice 1 -> negro lógico completamente opaco
#
# En nuestro SSD1306:
#
# negro lógico -> píxel físicamente encendido
#
# Por eso el logo aparecerá blanco sobre fondo negro.
# ============================================================

palette = bytearray([
    # --------------------------------
    # índice 0
    # transparente
    # --------------------------------
    0x00,
    0x00,
    0x00,
    0x00,

    # --------------------------------
    # índice 1
    # negro lógico opaco
    # --------------------------------
    0x00,
    0x00,
    0x00,
    0xFF,
])


# ============================================================
# DATOS COMPLETOS PARA LVGL
#
# [paleta][bitmap]
# ============================================================

data = palette + bitmap


# ============================================================
# GENERAR logo.h
# ============================================================

with open(
    OUTPUT,
    "w",
    encoding="utf-8"
) as f:

    f.write(
        "#pragma once\n\n"
    )

    f.write(
        "#include <lvgl.h>\n\n"
    )


    # --------------------------------------------------------
    # ARRAY DE PIXELES
    # --------------------------------------------------------

    f.write(
        "static const uint8_t adx_logo_map[] = {\n"
    )

    for i in range(
        0,
        len(data),
        16
    ):

        chunk = data[i:i + 16]

        f.write(
            "    "
        )

        f.write(
            ", ".join(
                f"0x{value:02X}"
                for value in chunk
            )
        )

        f.write(
            ",\n"
        )

    f.write(
        "};\n\n"
    )


    # --------------------------------------------------------
    # DESCRIPTOR LVGL 9
    # --------------------------------------------------------

    f.write(
        "static const lv_image_dsc_t adx_logo = {\n"
    )

    f.write(
        "    .header = {\n"
    )

    f.write(
        "        .magic = LV_IMAGE_HEADER_MAGIC,\n"
    )

    f.write(
        "        .cf = LV_COLOR_FORMAT_I1,\n"
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
        "    .data_size = sizeof(adx_logo_map),\n"
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
# INFORMACIÓN
# ============================================================

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
    f"Generado: {OUTPUT}"
)