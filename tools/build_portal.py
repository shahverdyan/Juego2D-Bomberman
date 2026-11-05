"""Extrae la animación de la puerta-portal del final del nivel.

La fuente es src/images/Portal_Rick_Gemini.jpeg, generada con IA: 7 fotogramas
en una cuadrícula de 4x2 (el último hueco está vacío) sobre fondo verde azulado.
Como es un JPEG grande, el script:
  1. Vuelve transparente el fondo (color parecido a (30, 89, 99)).
  2. Recorta el marco de piedra de cada fotograma. Los de la segunda fila son
     algo más grandes en la fuente: todos se reducen al mismo tamaño.
  3. Reduce cada fotograma a 28x32 píxeles con alfa binario.

Genera src/images/portal-atlas.png (7 celdas de 28x32 en una fila):
  0 cerrada, 1-4 abriéndose, 5-6 portal abierto girando.

Uso: python tools/build_portal.py
"""
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
IMAGES = ROOT / "src" / "images"
SOURCE = IMAGES / "Portal_Rick_Gemini.jpeg"
CELL_W, CELL_H = 28, 32
# Marco de piedra de cada fotograma en la fuente: (x0, y0, x1, y1).
# En el fotograma 1 se ignoran las chispas que caen por debajo del marco.
FRAMES = [(25, 25, 227, 252), (281, 25, 487, 252), (537, 25, 743, 252),
          (797, 25, 999, 252), (21, 298, 235, 547), (277, 298, 491, 547),
          (533, 298, 747, 547)]


def is_background(p):
    return abs(p[0] - 30) + abs(p[1] - 89) + abs(p[2] - 99) < 45


def cut(im, box):
    frame = im.crop(box).convert("RGBA")
    frame.putdata([(0, 0, 0, 0) if is_background(p) else p for p in frame.getdata()])
    small = frame.resize((CELL_W, CELL_H), Image.BOX)
    # Alfa binario para que no queden bordes semitransparentes.
    small.putalpha(small.getchannel("A").point(lambda a: 255 if a >= 128 else 0))
    return small


def main():
    im = Image.open(SOURCE).convert("RGB")
    atlas = Image.new("RGBA", (CELL_W * len(FRAMES), CELL_H), (0, 0, 0, 0))
    for i, box in enumerate(FRAMES):
        atlas.alpha_composite(cut(im, box), (i * CELL_W, 0))
    atlas.save(IMAGES / "portal-atlas.png")
    print("portal-atlas.png:", atlas.size)


if __name__ == "__main__":
    main()
