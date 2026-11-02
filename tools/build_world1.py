"""Extract exact 16x16 source pixels and assemble our own level, without repainting.
Requires Pillow. Run from any directory. The input image is kept unchanged.
"""
from pathlib import Path
import json
import argparse
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
TILES = ROOT / 'src/images/tilesets'
SOURCE = TILES / 'Game Boy _ GBC - Pocket Bomberman - Stages (World 1) - Stage 1-4.png'
# Source grid origin (15,13), tile size 16. Numbers are one-based atlas IDs.
CATALOG = [
    ('background', 2, 1), ('grass_left', 14, 2), ('grass_middle', 15, 2),
    ('grass_right', 19, 2), ('column_body', 5, 5), ('floor', 1, 14),
    ('waterfall_top', 7, 1), ('waterfall_middle', 7, 2), ('waterfall_bottom', 7, 7),
    ('foliage', 1, 1), ('border_left', 0, 1), ('border_right', 31, 1),
    ('border_top', 1, 0), ('border_top_left', 0, 0), ('border_top_right', 31, 0),
    ('door_marker', 26, 4),
]


def extract_atlas():
    original = Image.open(SOURCE).convert('RGBA')
    atlas = Image.new('RGBA', (64, 64))
    manifest = []
    for index, (name, column, row) in enumerate(CATALOG):
        x, y = 15 + column * 16, 13 + row * 16
        tile = original.crop((x, y, x + 16, y + 16))
        atlas.paste(tile, ((index % 4) * 16, (index // 4) * 16))
        manifest.append(dict(id=index + 1, name=name, source_rect=[x, y, 16, 16]))
    atlas.save(TILES / 'world1-atlas.png')
    (TILES / 'world1-atlas.json').write_text(json.dumps(dict(
        source=SOURCE.name, credit='Pocket Bomberman / Hudson Soft; supplied rip credited to Fret & Woppet',
        tile_size=16, columns=4, rows=4, tiles=manifest), indent=2) + '\n', encoding='utf-8')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--layout-only', action='store_true', help='Reuse existing atlas; regenerate only level and preview')
    args = parser.parse_args()
    if not args.layout_only:
        extract_atlas()
    atlas = Image.open(TILES / 'world1-atlas.png').convert('RGBA')

    width, height = 48, 26
    bg = [[1] * width for _ in range(height)]
    fg = [[0] * width for _ in range(height)]
    solid = [[0] * width for _ in range(height)]
    for y in range(height):
        fg[y][0], fg[y][-1] = 11, 12
        solid[y][0] = solid[y][-1] = 1
    for x in range(width):
        fg[0][x] = 13
        fg[-1][x] = 6
        solid[0][x] = solid[-1][x] = 1
    fg[0][0], fg[0][-1] = 14, 15

    def platform(left, right, top, bottom):
        for y in range(top, bottom + 1):
            for x in range(left, right + 1):
                fg[y][x] = (2 if x == left else 4 if x == right else 3) if y == top else 5
                solid[y][x] = 1

    # Western grove: a hollow arch, stepped interior and an optional low alcove.
    # Coordinates are tile coordinates. Each ascent is at most two tiles (64 units).
    for rect in [
        (3, 5, 23, 23),           # First easy jump from the clearing.
        (7, 18, 21, 22),          # Wide base; falling here lets the player try again.
        (7, 8, 11, 17),           # Western trunk of the arch.
        (5, 9, 10, 11),           # Overhanging canopy.
        (17, 18, 17, 21),         # Shorter eastern trunk: intentionally asymmetric.
        (10, 12, 19, 19), (14, 16, 17, 17),
        (10, 12, 15, 15), (14, 16, 13, 13),
        (10, 11, 11, 11),         # Branch back to the canopy.
        (2, 4, 18, 18),           # Side alcove reached by dropping from above.
        (19, 21, 13, 13), (17, 19, 11, 11),
    ]:
        platform(*rect)

    # Central ruin: open well with platforms inside, a side opening and a roof route.
    for rect in [
        (22, 34, 20, 21), (22, 23, 11, 20), (33, 34, 15, 20),
        (22, 30, 10, 11),
        (25, 27, 18, 18), (29, 31, 16, 16), (25, 26, 14, 14),
        (32, 34, 12, 12),         # Gap at the roof edge leads into the well.
    ]:
        platform(*rect)

    # Optional canopy loop: a high perch and a shortcut back toward the eastern ruin.
    for rect in [
        (28, 30, 8, 8), (24, 26, 6, 6),
        (28, 31, 4, 4),
        (33, 35, 6, 6),
    ]:
        platform(*rect)

    # Eastern ruin: zig-zag ascent inside a tall open arch; exit on its roof.
    for rect in [
        (36, 45, 21, 22), (44, 45, 10, 21), (43, 45, 8, 9),
        (38, 40, 19, 19), (41, 43, 17, 17),
        (37, 39, 15, 15), (41, 43, 13, 13),
        (37, 39, 11, 11), (40, 41, 10, 10),
        (35, 37, 23, 23),
    ]:
        platform(*rect)

    # Continuous lower passage allows recovery after falling from any of the ruins.
    # Waterfalls hang from overhangs, behind terrain, rather than floating in open sky.
    for x, top, bottom in [(9, 12, 20), (27, 12, 19), (40, 11, 20)]:
        bg[top][x] = 7
        for y in range(top + 1, bottom):
            bg[y][x] = 8
        bg[bottom][x] = 9

    # Foliage sits on platforms and in side pockets; it never blocks movement.
    for x, y in [(3,24), (4,22), (11,20), (15,16), (20,12),
                 (6,9), (11,10), (23,9), (29,9), (26,19), (29,15),
                 (33,11), (36,20), (39,18), (42,16), (38,10), (45,7), (30,3), (34,5), (29,7)]:
        if fg[y][x] == 0:
            bg[y][x] = 10
    # Visual exit marker; level progression is implemented in a later phase.
    fg[7][43] = 16

    lines = ['TILEMAP2', f'{width} {height}', '32 32', 'images/tilesets/world1-atlas.png', '4 4', 'SPAWN 2 24']
    for name, layer in [('BACKGROUND', bg), ('FOREGROUND', fg)]:
        lines.append(name)
        lines.extend(' '.join(map(str, row)) for row in layer)
    lines.append('COLLISION')
    lines.extend(''.join(map(str, row)) for row in solid)
    (ROOT / 'src/levels/level01.txt').write_text('\n'.join(lines) + '\n', encoding='utf-8')

    # Whole-level preview at source resolution, separate from the live camera.
    preview = Image.new('RGBA', (width * 16, height * 16))
    for layer in [bg, fg]:
        for y, row in enumerate(layer):
            for x, tile_id in enumerate(row):
                if tile_id:
                    i = tile_id - 1
                    tile = atlas.crop(((i % 4)*16, (i // 4)*16, (i % 4+1)*16, (i // 4+1)*16))
                    preview.alpha_composite(tile, (x*16, y*16))
    preview.save(ROOT / 'docs/level01-preview.png')
    print('Generated level01: 48x26, three connected ruins, lower recovery passage and roof exit.')


if __name__ == '__main__':
    main()
