"""Extrae los fotogramas del enemigo Boo.

La fuente es la hoja custom de Teeks (de terceros, no de Bomberman), en
píxeles nativos sobre fondo blanco. El cuerpo del Boo también es blanco, así
que el fondo se quita rellenando desde el borde de cada recorte: el contorno
gris del Boo frena el relleno y el interior conserva su blanco.

Genera src/images/boo-atlas.png con celdas de 20x16:
  fila 0 mira a la izquierda (como la fuente), fila 1 es el espejo.
  columnas 0-3 persiguiendo, columna 4 escondido (se tapa la cara),
  columna 5 ojos cerrados (muerte, de la tercera fila de la hoja).

Uso: python tools/build_boo.py
"""
from collections import deque
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
IMAGES = ROOT / "src" / "images"
SOURCE = IMAGES / "Custom _ Edited - Mario Customs - Boo & Related Enemies - Boo.png"
CELL_W, CELL_H = 20, 16
# (x, y) de la esquina de cada fotograma de 20x16.
FRAMES = [(7, 3), (29, 3), (53, 3), (78, 3), (103, 3), (6, 52)]
WHITE = (255, 255, 255)


def cut(im, x0, y0):
    """Recorta una celda y vuelve transparente el blanco conectado al borde."""
    cell = im.crop((x0, y0, x0 + CELL_W, y0 + CELL_H)).convert("RGBA")
    px = cell.load()
    queue = deque()
    for x in range(CELL_W):
        queue.extend([(x, 0), (x, CELL_H - 1)])
    for y in range(CELL_H):
        queue.extend([(0, y), (CELL_W - 1, y)])
    while queue:
        x, y = queue.popleft()
        if not (0 <= x < CELL_W and 0 <= y < CELL_H):
            continue
        if px[x, y][:3] != WHITE or px[x, y][3] == 0:
            continue
        px[x, y] = (0, 0, 0, 0)
        queue.extend([(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)])
    return cell


def main():
    im = Image.open(SOURCE).convert("RGB")
    atlas = Image.new("RGBA", (CELL_W * len(FRAMES), CELL_H * 2), (0, 0, 0, 0))
    for i, (x, y) in enumerate(FRAMES):
        cell = cut(im, x, y)
        atlas.alpha_composite(cell, (i * CELL_W, 0))
        atlas.alpha_composite(cell.transpose(Image.FLIP_LEFT_RIGHT), (i * CELL_W, CELL_H))
    atlas.save(IMAGES / "boo-atlas.png")
    print("boo-atlas.png:", atlas.size)


if __name__ == "__main__":
    main()
