"""Extrae la animación de caminar del Koopa Troopa para el enemigo Walker.

La fuente es la hoja custom de NO Body (de terceros, no de Bomberman), en
píxeles nativos sobre un fondo azul grisáceo uniforme. Se usan los 8
fotogramas de caminar de la segunda fila (grupo derecho, Koopa verde).
Debajo hay una línea roja que separa grupos: se recorta por encima de ella.

Genera src/images/koopa-atlas.png con celdas de 32x32 (escala 1:1, el
Koopa centrado y apoyado abajo):
  fila 0 mira a la izquierda (espejo), fila 1 mira a la derecha (como la fuente).
  fila 2 muerte: 0 agachado, 1 dentro del caparazón, 2 caparazón boca abajo.

Uso: python tools/build_koopa.py
"""
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
IMAGES = ROOT / "src" / "images"
SOURCE = IMAGES / ("Custom _ Edited - Mario Customs - Koopa Troopa & "
                   "Related Enemies - Koopa Troopa.png")
CELL = 32
BACKGROUND = (64, 72, 96)
TOP, BOTTOM = 59, 90  # filas de la hoja que ocupa el ciclo de caminar
# Columnas [x0, x1) de cada fotograma de caminar.
FRAMES = [(257, 278), (280, 303), (304, 328), (330, 353),
          (356, 377), (378, 401), (405, 428), (433, 454)]


# Fotogramas de la muerte (x0, y0, x1, y1), en la fila de los caparazones.
DEATH_FRAMES = [(232, 111, 259, 130), (264, 115, 281, 130), (349, 112, 366, 127)]


def cut(im, x0, x1, top=TOP, bottom=BOTTOM, upside_down=False):
    """Recorta un fotograma y vuelve transparente el color de fondo.
    upside_down: lo gira boca abajo antes de apoyarlo en la celda."""
    frame = im.crop((x0, top, x1, bottom)).convert("RGBA")
    if upside_down:
        frame = frame.transpose(Image.FLIP_TOP_BOTTOM)
    frame.putdata([(0, 0, 0, 0) if p[:3] == BACKGROUND else p for p in frame.getdata()])
    cell = Image.new("RGBA", (CELL, CELL), (0, 0, 0, 0))
    cell.alpha_composite(frame, ((CELL - frame.width) // 2, CELL - frame.height))
    return cell


def main():
    im = Image.open(SOURCE).convert("RGB")
    atlas = Image.new("RGBA", (CELL * len(FRAMES), CELL * 3), (0, 0, 0, 0))
    for i, (x0, x1) in enumerate(FRAMES):
        cell = cut(im, x0, x1)
        atlas.alpha_composite(cell.transpose(Image.FLIP_LEFT_RIGHT), (i * CELL, 0))
        atlas.alpha_composite(cell, (i * CELL, CELL))
    for i, (x0, y0, x1, y1) in enumerate(DEATH_FRAMES):
        cell = cut(im, x0, x1, y0, y1, upside_down=(i == 2))
        atlas.alpha_composite(cell, (i * CELL, 2 * CELL))
    atlas.save(IMAGES / "koopa-atlas.png")
    print("koopa-atlas.png:", atlas.size)


if __name__ == "__main__":
    main()
