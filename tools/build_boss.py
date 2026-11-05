"""Extrae los sprites del jefe final, sus bolas de fuego y sus gárgolas.

Fuente: hoja original "Pocket Bomberman - Characters - Overworld (Color)" (GIF en
píxeles nativos de Game Boy, fondo azul uniforme). Última fila de la hoja:
el jefe del mundo Evil, sus bolas de fuego y las gárgolas que invoca.

Genera, con el fondo transparente:
  src/images/boss-atlas.png      4 fotogramas de 32x32: 0-1 reposo, 2 alas
                                 levantadas (ataque), 3 herido.
  src/images/boss-fireball.png   6 fotogramas de 16x16: 0-2 bola latiendo,
                                 3-5 estallido.
  src/images/gargoyle-atlas.png  3 fotogramas de 16x16: aleteo.

Uso: python tools/build_boss.py
"""
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
IMAGES = ROOT / "src" / "images"
SOURCE = IMAGES / "Game Boy _ GBC - Pocket Bomberman - Characters - Overworld (Color).gif"
BACKGROUND = (107, 94, 236)

# x de la esquina de cada fotograma; todos en la misma fila.
BOSS_X, BOSS_Y, BOSS_SIZE = [2, 35, 68, 102], 597, 32
FIREBALL_X, FIREBALL_Y = [136, 154, 171, 188, 206, 224], 596
GARGOYLE_X, GARGOYLE_Y = [136, 154, 172], 614
SMALL_SIZE = 16


def strip(sheet, xs, y, size):
    """Pone en fila los fotogramas indicados y vuelve transparente el fondo."""
    atlas = Image.new("RGBA", (size * len(xs), size), (0, 0, 0, 0))
    for i, x in enumerate(xs):
        atlas.paste(sheet.crop((x, y, x + size, y + size)), (i * size, 0))
    atlas.putdata([(0, 0, 0, 0) if p[:3] == BACKGROUND else p for p in atlas.getdata()])
    return atlas


def main():
    sheet = Image.open(SOURCE).convert("RGBA")
    strip(sheet, BOSS_X, BOSS_Y, BOSS_SIZE).save(IMAGES / "boss-atlas.png")
    strip(sheet, FIREBALL_X, FIREBALL_Y, SMALL_SIZE).save(IMAGES / "boss-fireball.png")
    strip(sheet, GARGOYLE_X, GARGOYLE_Y, SMALL_SIZE).save(IMAGES / "gargoyle-atlas.png")
    print("Generados boss-atlas.png, boss-fireball.png y gargoyle-atlas.png")


if __name__ == "__main__":
    main()
