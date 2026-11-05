"""Genera src/images/rick-atlas.png, el spritesheet del jugador (Rick).

Fuente: hoja custom de Rick & Morty (Mister Mike, de terceros, no de Bomberman).
Se lee la versión original 851x84 guardada en el commit 95db676, porque la
copia de trabajo actual es una captura reescalada x1.2 con fondo de cuadros.

Cada fotograma es una celda de 24x24 con fondo azul claro; los dos azules del
fondo pasan a transparentes. Atlas resultante: 6 columnas x 2 filas de 24x24.
  Fila 0 mira a la derecha (como la fuente), fila 1 es el espejo (izquierda).
  Columnas: 0 quieto, 1-3 caminar, 4 subiendo (salto), 5 cayendo, 6 muerte (susto).

Uso: python tools/build_player.py
"""
import io
import subprocess
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ("src/images/Custom _ Edited - Rick and Morty Customs - The Smith Family - "
          "Rick & Morty (Super Mario Maker-Style).png")
SOURCE_COMMIT = "95db676"
OUTPUT = ROOT / "src" / "images" / "rick-atlas.png"
CELL = 24
FRAME_X = [1, 101, 126, 151, 351, 401, 776]  # x de cada celda en la fila de Rick (y = 1)
BACKGROUND = {(50, 97, 168), (147, 187, 236)}


def main():
    data = subprocess.run(["git", "show", f"{SOURCE_COMMIT}:{SOURCE}"],
                          cwd=ROOT, capture_output=True, check=True).stdout
    sheet = Image.open(io.BytesIO(data)).convert("RGBA")
    atlas = Image.new("RGBA", (CELL * len(FRAME_X), CELL * 2), (0, 0, 0, 0))
    for i, x in enumerate(FRAME_X):
        frame = sheet.crop((x, 1, x + CELL, 1 + CELL))
        frame.putdata([(0, 0, 0, 0) if p[:3] in BACKGROUND else p for p in frame.getdata()])
        atlas.alpha_composite(frame, (i * CELL, 0))
        atlas.alpha_composite(frame.transpose(Image.FLIP_LEFT_RIGHT), (i * CELL, CELL))
    atlas.save(OUTPUT)
    print("Generado", OUTPUT)


if __name__ == "__main__":
    main()
