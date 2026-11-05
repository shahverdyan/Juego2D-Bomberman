"""Genera la arena provisional del jefe: src/levels/level05.txt (y level05.tmj).

Sala cerrada de 10x9 tiles (una sola pantalla, como las salas de jefe de Game
Boy) con los tiles del mundo 1: suelo abajo, dos plataformas para esquivar, un
arbusto con Fire Up y otro con Bomb Up, y el marcador de la puerta en el centro
del suelo (la puerta se abre al vencer al jefe). Es provisional: se puede rediseñar en Tiled abriendo level05.tmj y
convirtiéndolo con tools/tiled_map.py to-game. Ojo: volver a ejecutar este
script sobrescribe level05.txt y level05.tmj.

Uso: python tools/build_boss_arena.py
"""
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LEVELS = ROOT / "src" / "levels"
WIDTH, HEIGHT = 10, 9
SPAWN = (1, 7)

# IDs del atlas World 1 (src/images/tilesets/world1-atlas.json).
BACKGROUND, GRASS_LEFT, GRASS_MIDDLE, GRASS_RIGHT, FLOOR = 1, 2, 3, 4, 6
FOLIAGE, BORDER_LEFT, BORDER_RIGHT, BORDER_TOP = 10, 11, 12, 13
BORDER_TOP_LEFT, BORDER_TOP_RIGHT, DOOR_MARKER = 14, 15, 16
FIRE_UP, BOMB_UP = 0, 1

PLATFORMS = [(1, 5, 2), (7, 5, 2)]        # (columna, fila, longitud)
BUSHES = [(1, 4, FIRE_UP), (8, 4, BOMB_UP)]   # arbusto con objeto encima de cada plataforma
DOOR = (4, 7)                             # casilla inferior izquierda de la puerta de 2x2


def main():
    background = [[BACKGROUND] * WIDTH for _ in range(HEIGHT)]
    foreground = [[0] * WIDTH for _ in range(HEIGHT)]
    collision = [[0] * WIDTH for _ in range(HEIGHT)]

    # Bordes: techo arriba, paredes a los lados y suelo abajo (todo sólido).
    for x in range(WIDTH):
        foreground[0][x] = BORDER_TOP
        foreground[HEIGHT - 1][x] = FLOOR
        collision[0][x] = collision[HEIGHT - 1][x] = 1
    foreground[0][0], foreground[0][WIDTH - 1] = BORDER_TOP_LEFT, BORDER_TOP_RIGHT
    for y in range(1, HEIGHT - 1):
        foreground[y][0], foreground[y][WIDTH - 1] = BORDER_LEFT, BORDER_RIGHT
        collision[y][0] = collision[y][WIDTH - 1] = 1

    for x0, y, length in PLATFORMS:
        for i in range(length):
            foreground[y][x0 + i] = GRASS_LEFT if i == 0 else GRASS_RIGHT if i == length - 1 else GRASS_MIDDLE
            collision[y][x0 + i] = 1
    # Los arbustos son sólidos por ser follaje; su colisión queda a 0 para que
    # desaparezca al destruirlos, como en el nivel 1.
    for x, y, _ in BUSHES:
        background[y][x] = FOLIAGE
    foreground[DOOR[1]][DOOR[0]] = DOOR_MARKER

    lines = ["TILEMAP2", f"{WIDTH} {HEIGHT}", "32 32", "images/tilesets/world1-atlas.png", "4 4",
             f"SPAWN {SPAWN[0]} {SPAWN[1]}", "BACKGROUND"]
    lines += [" ".join(map(str, row)) for row in background]
    lines.append("FOREGROUND")
    lines += [" ".join(map(str, row)) for row in foreground]
    lines.append("COLLISION")
    lines += ["".join(map(str, row)) for row in collision]
    lines.append(f"ITEMS {len(BUSHES)}")
    lines += [f"{x} {y} {item}" for x, y, item in BUSHES]
    (LEVELS / "level05.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")

    # Versión para Tiled, con el conversor del proyecto.
    subprocess.run([sys.executable, str(ROOT / "tools" / "tiled_map.py"), "to-tiled",
                    str(LEVELS / "level05.txt"), str(LEVELS / "level05.tmj")], check=True)
    print("Generados level05.txt y level05.tmj")


if __name__ == "__main__":
    main()
