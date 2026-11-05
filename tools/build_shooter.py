"""Extrae los fotogramas del enemigo Shooter (Sneed) y su bala.

La imagen fuente es una captura RGB con el fondo de cuadros grises/blancos
"pintado" y escalada con pérdida. El script:
  1. Rellena desde los bordes los píxeles grises claros (fondo y líneas
     divisorias). El contorno negro del personaje frena el relleno.
  2. Separa los fotogramas como componentes conexas.
  3. Reduce cada fotograma a celdas de 32x48 (bala: 16x16) con alfa binario.
  4. Genera src/images/shooter-atlas.png: fila 0 mira a la izquierda,
     fila 1 es el espejo (mira a la derecha). Columnas:
       0-3 caminar, 4-5 alerta, 6-7 disparo.
     Fila 2: muerte, el fotograma de alerta boca abajo (col 0 izquierda, col 1 derecha).
     Y src/images/shooter-bullet.png.

Uso: python tools/build_shooter.py
"""
from collections import deque
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
IMAGES = ROOT / "src" / "images"
SOURCE = IMAGES / ("Custom _ Edited - Mario & Luigi Customs - Bowser_s Inside "
                   "Story Enemies - Sneed (The Simpsons-Style).png")
CELL_W, CELL_H = 32, 48
BULLET = 16


def is_background(p):
    # Fondo: gris neutro claro. El blanco del personaje tiene tinte cian
    # (245,255,255), así que se excluye para que el relleno no lo invada.
    r, g, b = p
    return max(r, g, b) - min(r, g, b) <= 14 and min(r, g, b) >= 150 and g - r <= 4


def sprite_mask(im):
    w, h = im.size
    px = im.load()
    outside = [[False] * w for _ in range(h)]
    queue = deque()
    for x in range(w):
        queue.extend([(x, 0), (x, h - 1)])
    for y in range(h):
        queue.extend([(0, y), (w - 1, y)])
    while queue:
        x, y = queue.popleft()
        if not (0 <= x < w and 0 <= y < h) or outside[y][x]:
            continue
        if not is_background(px[x, y]):
            continue
        outside[y][x] = True
        queue.extend([(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)])
    return [[not outside[y][x] for x in range(w)] for y in range(h)]


def components(mask, min_pixels=2000):
    h, w = len(mask), len(mask[0])
    seen = [[False] * w for _ in range(h)]
    boxes = []
    for sy in range(h):
        for sx in range(w):
            if not mask[sy][sx] or seen[sy][sx]:
                continue
            queue, count = deque([(sx, sy)]), 0
            seen[sy][sx] = True
            x0 = x1 = sx
            y0 = y1 = sy
            while queue:
                x, y = queue.popleft()
                count += 1
                x0, x1, y0, y1 = min(x0, x), max(x1, x), min(y0, y), max(y1, y)
                for nx, ny in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
                    if 0 <= nx < w and 0 <= ny < h and mask[ny][nx] and not seen[ny][nx]:
                        seen[ny][nx] = True
                        queue.append((nx, ny))
            if count >= min_pixels:
                boxes.append((x0, y0, x1 + 1, y1 + 1))
    return boxes


def cut(im, mask, box):
    x0, y0, x1, y1 = box
    out = Image.new("RGBA", (x1 - x0, y1 - y0), (0, 0, 0, 0))
    for y in range(y0, y1):
        for x in range(x0, x1):
            if mask[y][x]:
                out.putpixel((x - x0, y - y0), im.getpixel((x, y)) + (255,))
    return out


def shrink(frame, scale, cell_w, cell_h):
    """Reduce con filtro caja y coloca el fotograma centrado y apoyado abajo."""
    w = max(1, round(frame.width / scale))
    h = max(1, round(frame.height / scale))
    small = frame.resize((w, h), Image.BOX)
    # Alfa binario para que el blending no deje halos grises.
    small.putalpha(small.getchannel("A").point(lambda a: 255 if a >= 128 else 0))
    cell = Image.new("RGBA", (cell_w, cell_h), (0, 0, 0, 0))
    cell.alpha_composite(small, ((cell_w - w) // 2, cell_h - h))
    return cell


def main():
    im = Image.open(SOURCE).convert("RGB")
    mask = sprite_mask(im)
    boxes = components(mask)
    bullet_box = min(boxes, key=lambda b: (b[2] - b[0]) * (b[3] - b[1]))
    sneeds = [b for b in boxes if b != bullet_box]
    if len(sneeds) != 8:
        raise SystemExit(f"Se esperaban 8 fotogramas, encontrados {len(sneeds)}")
    # Orden de lectura: por filas (centro vertical) y luego de izquierda a derecha.
    # Las tres filas de la hoja miden unos 290 px de alto.
    sneeds.sort(key=lambda b: (int((b[1] + b[3]) / 2 // 290), b[0]))

    scale = max(b[3] - b[1] for b in sneeds) / (CELL_H - 2)
    atlas = Image.new("RGBA", (CELL_W * 8, CELL_H * 3), (0, 0, 0, 0))
    for i, box in enumerate(sneeds):
        cell = shrink(cut(im, mask, box), scale, CELL_W, CELL_H)
        atlas.alpha_composite(cell, (i * CELL_W, 0))
        atlas.alpha_composite(cell.transpose(Image.FLIP_LEFT_RIGHT), (i * CELL_W, CELL_H))
        if i == 4:
            dead = cell.transpose(Image.FLIP_TOP_BOTTOM)
            atlas.alpha_composite(dead, (0, 2 * CELL_H))
            atlas.alpha_composite(dead.transpose(Image.FLIP_LEFT_RIGHT), (CELL_W, 2 * CELL_H))
    atlas.save(IMAGES / "shooter-atlas.png")

    bullet = cut(im, mask, bullet_box)
    side = max(bullet.size)
    square = Image.new("RGBA", (side, side), (0, 0, 0, 0))
    square.alpha_composite(bullet, ((side - bullet.width) // 2, (side - bullet.height) // 2))
    shrink(square, side / (BULLET - 4), BULLET, BULLET).save(IMAGES / "shooter-bullet.png")
    print("Fotogramas:", sneeds, "bala:", bullet_box, "escala:", round(scale, 2))


if __name__ == "__main__":
    main()
