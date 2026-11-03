"""Genera src/images/items-atlas.png: los 4 objetos que salen de los arbustos.

Atlas de 4 celdas de 32x32 en una fila, en el orden del enum ItemType (Item.h):
  0 Fire Up   <- images/Fire_UP_gemini.jpeg (imagen generada con IA)
  1 Bomb Up   <- images/Bomb_UP_gemini.jpeg (imagen generada con IA)
  2 Corazón   <- images/Pixel Heart Sprite Sheet 16x16.png (corazón lleno)
  3 Estrella  <- images/varied.png (celda inferior derecha)

Las imágenes de Gemini son JPEG con el fondo de cuadros y un halo pintados.
El contorno oscuro del dibujo hace de barrera: se rellena el fondo desde una
esquina y lo que queda dentro del contorno es el objeto (ver cut_gemini).

Uso: python tools/build_items.py
"""
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter

ROOT = Path(__file__).resolve().parent.parent
IMAGES = ROOT / "src" / "images"
OUTPUT = IMAGES / "items-atlas.png"
CELL = 32


def is_outline(p):
    # Contorno de los dibujos: oscuro y con dominante roja, p. ej. (65,27,26).
    r, g, b = p
    return max(r, g, b) < 110 and r - g > 20


def cut_gemini(path, seal=9):
    """Separa el dibujo del fondo usando su contorno como barrera.

    El contorno del JPEG tiene huecos, así que primero se engorda (MaxFilter)
    para cerrarlos, se rellena el fondo desde una esquina y al final se
    adelgaza la máscara (MinFilter) para volver al borde original.
    """
    im = Image.open(path).convert("RGB")
    barrier = Image.new("L", im.size, 0)
    barrier.putdata([255 if is_outline(p) else 0 for p in im.getdata()])
    barrier = barrier.filter(ImageFilter.MaxFilter(seal))
    ImageDraw.floodfill(barrier, (0, 0), 128)
    inside = barrier.point(lambda v: 0 if v == 128 else 255)
    inside = inside.filter(ImageFilter.MinFilter(seal))
    out = im.convert("RGBA")
    out.putalpha(inside)
    return out.crop(inside.getbbox())


def fit(sprite, margin=2):
    """Reduce con filtro caja a la celda de 32x32, centrado y con alfa binario."""
    side = CELL - 2 * margin
    scale = max(sprite.width, sprite.height) / side
    w, h = max(1, round(sprite.width / scale)), max(1, round(sprite.height / scale))
    small = sprite.resize((w, h), Image.BOX)
    small.putalpha(small.getchannel("A").point(lambda a: 255 if a >= 128 else 0))
    cell = Image.new("RGBA", (CELL, CELL), (0, 0, 0, 0))
    cell.alpha_composite(small, ((CELL - w) // 2, (CELL - h) // 2))
    return cell


def pixel_art(path, box, factor):
    """Recorte de pixel art ya limpio, ampliado con vecino más cercano."""
    sprite = Image.open(path).convert("RGBA").crop(box)
    sprite = sprite.crop(sprite.getbbox())
    big = sprite.resize((sprite.width * factor, sprite.height * factor), Image.NEAREST)
    cell = Image.new("RGBA", (CELL, CELL), (0, 0, 0, 0))
    cell.alpha_composite(big, ((CELL - big.width) // 2, (CELL - big.height) // 2))
    return cell


def main():
    cells = [
        fit(cut_gemini(IMAGES / "Fire_UP_gemini.jpeg")),
        fit(cut_gemini(IMAGES / "Bomb_UP_gemini.jpeg")),
        pixel_art(IMAGES / "Pixel Heart Sprite Sheet 16x16.png", (0, 0, 16, 16), 2),
        pixel_art(IMAGES / "varied.png", (32, 32, 64, 64), 2),
    ]
    atlas = Image.new("RGBA", (CELL * len(cells), CELL), (0, 0, 0, 0))
    for i, cell in enumerate(cells):
        atlas.alpha_composite(cell, (i * CELL, 0))
    atlas.save(OUTPUT)
    print("Generado", OUTPUT)


if __name__ == "__main__":
    main()
