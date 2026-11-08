"""Genera las pantallas de instrucciones, créditos y selección de nivel, y sus sprites.

El fondo del menú principal (menu-background.jpeg), game-over.jpeg y victory.png
son imágenes hechas a mano: este script no las genera.

Piezas usadas:
  - Escena del árbol con Rick: GEMINI_Fotos_Menus_RICK.jpeg (generada con IA a
    partir de la hoja de menús).
  - Bomba-cursor y selección de mundo: hoja original de menús de Pocket Bomberman
    (PNG en píxeles nativos de Game Boy).
  - Recuadros: mismos colores que el recuadro original (morado, amarillo, negro).
  - Texto: la fuente 5x7 de tools/build_hud_font.py, más flechas y una W más clara.

Pantallas a escala 2 (320x304, texto pequeño):
  menu-instructions.png  controles y objetivo (texto en INSTRUCTIONS).
  menu-credits.png       autores y origen de los gráficos (texto en CREDITS).
Pantalla a escala 4 (160x152, mismo tamaño de píxel que la partida):
  level-select.png       selección de mundo original con una franja de ayuda.
Sprites:
  menu-cursor.png        bomba roja de 8x8 (Menu.cpp la mueve sobre las opciones).
  level-buttons.png      botones de mundo de 56x32: fila 0 normal, fila 1 elegido,
                         columnas en orden de nivel (Forest I a Forest V).

Las posiciones de los botones deben coincidir con LevelSelect.cpp.

Uso: python tools/build_menu.py
"""
from pathlib import Path

from PIL import Image

from build_hud_font import GLYPHS

ROOT = Path(__file__).resolve().parent.parent
IMAGES = ROOT / "src" / "images"
RICK_SHEET = IMAGES / "GEMINI_Fotos_Menus_RICK.jpeg"
ORIGINAL_SHEET = IMAGES / "Game Boy _ GBC - Pocket Bomberman - Miscellaneous - Menus (Color).png"

PURPLE, YELLOW, BLACK, WHITE = (112, 112, 248), (248, 248, 0), (24, 24, 24), (248, 248, 248)
DARK_PURPLE = (48, 48, 128)  # fondo de los recuadros con mucho texto (más contraste)

# Glifos que no tiene la fuente del HUD: flechas, y una W que no parezca una U.
EXTRA_GLYPHS = {
    "<": [".....", "..#..", ".#...", "#####", ".#...", "..#..", "....."],
    ">": [".....", "..#..", "...#.", "#####", "...#.", "..#..", "....."],
    "^": ["..#..", ".###.", "#.#.#", "..#..", "..#..", "..#..", "....."],
    "W": ["#...#", "#...#", "#...#", "#.#.#", "#.#.#", "##.##", "#...#"],
}
FONT = {**GLYPHS, **EXTRA_GLYPHS}

# --- Instrucciones y créditos (escala 2) ---
INFO_BOX = (10, 10, 310, 294)
INSTRUCTIONS = [
    "INSTRUCCIONES",
    "",
    "< > O A D: MOVERSE",
    "^ O W: SALTAR",
    "ESPACIO: PONER BOMBA",
    "",
    "PUEDES SUBIRTE ENCIMA DE TUS BOMBAS",
    "EL FUEGO ROMPE ARBUSTOS Y CASCADAS",
    "Y DESCUBRE OBJETOS ESCONDIDOS",
    "",
    "ELIMINA A TODOS LOS ENEMIGOS PARA",
    "ABRIR EL PORTAL Y ENTRA EN EL",
    "ANTES DE QUE ACABE EL TIEMPO",
    "",
    "G: INVULNERABLE",
    "K: ELIMINAR ENEMIGOS",
    "R: VOLVER AL INICIO",
    "ESC: VOLVER AL MENU",
]
CREDITS = [
    "CREDITOS",
    "",
    "PROGRAMACION",
    "CRISTIAN CONRADO",
    "ALEKS",
    "",
    "POCKET BOMBERMAN: HUDSON SOFT",
    "TILES Y MENUS: FRET Y WOPPET",
    "RICK: MISTER MIKE",
    "BOO: TEEKS",
    "KOOPA: NO BODY",
    "SPRITES: THE SPRITERS RESOURCE",
    "SPRITES CUSTOM: HECHOS CON GEMINI",
    "IMAGENES DE RICK GENERADAS CON IA",
    "",
    "MUSICA: LOUSWAN  MRPOLY  MATTHEW PABLO",
    "KISTOL  JOBROMEDIA  JKFITE01",
    "SONIDOS: VIRCON32  THEMIGHTYGLIDER",
    "LUKE RUSTLTD  FUPI  LEOHPAZ",
    "",
    "VIDEOJOCS FIB UPC 2026 2027",
    "",
    "ESC: VOLVER AL MENU",
]

# --- Selección de nivel (escala 4) ---
WORLD_SCREEN = (230, 18, 390, 162)  # pantalla original de 160x144
# Los botones originales aportan los marcos y el color de cada nivel.
BUTTON_SOURCES = ("FOREST", "OCEAN", "WIND", "CLOUD", "EVIL")
LEVEL_LABELS = ("Forest I", "Forest II", "Forest III", "Forest IV", "Forest V")
BUTTON_POSITIONS = ((104, 96), (0, 80), (104, 56), (0, 32), (104, 16))
NORMAL_BUTTONS = {"OCEAN": (402, 18), "CLOUD": (402, 58), "EVIL": (402, 98),
                  "WIND": (402, 138), "FOREST": (402, 178)}
SELECTED_BUTTONS = {"EVIL": (470, 18), "WIND": (470, 57), "FOREST": (470, 96),
                    "CLOUD": (470, 135), "OCEAN": (470, 174)}
BUTTON_W, BUTTON_H = 56, 32
BUTTON_INK = (
    ((176, 0, 0), (248, 104, 0)),
    ((0, 0, 144), (0, 80, 208)),
    ((0, 0, 144), (0, 80, 208)),
    ((0, 88, 0), (0, 176, 0)),
    ((176, 0, 0), (248, 104, 0)),
)
# Letra compacta de 9 píxeles de alto para el rótulo de 40 píxeles.
LEVEL_GLYPHS = {
    "F": ("####", "####", "#...", "#...", "###.", "#...", "#...", "#...", "#..."),
    "o": ("....", "....", "....", ".##.", "#..#", "#..#", "#..#", "#..#", ".##."),
    "r": ("...", "...", "...", "#.#", "##.", "#..", "#..", "#..", "#.."),
    "e": ("....", "....", "....", ".##.", "#..#", "####", "#...", "#...", ".###"),
    "s": ("...", "...", "...", ".##", "#..", ".#.", "..#", "..#", "##."),
    "t": (".#.", ".#.", ".#.", "###", ".#.", ".#.", ".#.", ".#.", "..#"),
    "I": ("#",) * 9,
    "V": ("#...#", "#...#", "#...#", "#...#", ".#.#.", ".#.#.", ".#.#.", "..#..", "..#.."),
    " ": (".",) * 9,
}


def font_text_width(text, glyphs=FONT):
    return sum(len(glyphs[char][0]) + 1 for char in text)


def draw_text(image, text, x, y, color, glyphs=FONT, shadow=BLACK):
    for char in text:
        glyph = glyphs[char]
        for row, line in enumerate(glyph):
            for column, pixel in enumerate(line):
                if pixel == "#":
                    image.putpixel((x + column + 1, y + row + 1), shadow + (255,))
        for row, line in enumerate(glyph):
            for column, pixel in enumerate(line):
                if pixel == "#":
                    image.putpixel((x + column, y + row), color + (255,))
        x += len(glyph[0]) + 1


def draw_centered(image, text, y, color):
    draw_text(image, text, (image.width - font_text_width(text)) // 2, y, color)


def draw_box(image, box, fill=PURPLE):
    x0, y0, x1, y1 = box
    for inset, color in enumerate([PURPLE, YELLOW, BLACK]):
        for x in range(x0 + inset, x1 - inset):
            image.putpixel((x, y0 + inset), color + (255,))
            image.putpixel((x, y1 - 1 - inset), color + (255,))
        for y in range(y0 + inset, y1 - inset):
            image.putpixel((x0 + inset, y), color + (255,))
            image.putpixel((x1 - 1 - inset, y), color + (255,))
    for y in range(y0 + 3, y1 - 3):
        for x in range(x0 + 3, x1 - 3):
            image.putpixel((x, y), fill + (255,))


def tree_scene():
    """Escena del árbol con Rick reducida a 160x152 (resolución de Game Boy)."""
    sheet = Image.open(RICK_SHEET).convert("RGB")
    # Se recortan 2 px de margen para no coger el verde de la hoja. El filtro caja
    # conserva los colores de la cara de Rick; reducir la paleta los estropeaba.
    return sheet.crop((870, 18, 1052, 181)).resize((160, 152), Image.BOX).convert("RGBA")


def scale2(image):
    return image.resize((image.width * 2, image.height * 2), Image.NEAREST)


def cut_original(box, transparent):
    piece = Image.open(ORIGINAL_SHEET).convert("RGBA").crop(box)
    piece.putdata([(0, 0, 0, 0) if p[:3] == transparent else p for p in piece.getdata()])
    return piece


def hint_band(image, text):
    """Franja negra de 8 px abajo con una ayuda de teclas."""
    for y in range(144, 152):
        for x in range(image.width):
            image.putpixel((x, y), BLACK + (255,))
    draw_centered(image, text, 144, WHITE)


def menu_cursor():
    # Bomba roja de la hoja original; su fondo morado claro pasa a transparente.
    cut_original((42, 213, 50, 221), (120, 120, 248)).save(IMAGES / "menu-cursor.png")


def info_screen(lines, filename, spacing=14):
    """Escena del árbol con un recuadro grande: título amarillo y texto blanco centrados.
    spacing: píxeles entre líneas (los créditos van más juntos para que quepan)."""
    screen = scale2(tree_scene())
    draw_box(screen, INFO_BOX, DARK_PURPLE)
    y = INFO_BOX[1] + 14
    for i, line in enumerate(lines):
        draw_centered(screen, line, y, YELLOW if i == 0 else WHITE)
        y += spacing
    screen.convert("RGB").save(IMAGES / filename)


def label_button(button, label, colors, selected=False):
    # La placa blanca sube un píxel en el sprite seleccionado.
    top = 7 if selected else 8
    button.paste(WHITE + (255,), (8, top, 48, top + 16))
    width = font_text_width(label, LEVEL_GLYPHS)
    if width > 40:
        raise ValueError(f"El rótulo {label!r} no cabe en el botón")
    draw_text(button, label, 8 + (40 - width) // 2, top + 3, colors[0],
              glyphs=LEVEL_GLYPHS, shadow=colors[1])


def level_select():
    screen = Image.new("RGBA", (160, 152), BLACK + (255,))
    sheet = Image.open(ORIGINAL_SHEET).convert("RGBA")
    screen.paste(sheet.crop(WORLD_SCREEN), (0, 0))
    buttons = Image.new("RGBA", (BUTTON_W * len(LEVEL_LABELS), BUTTON_H * 2), (0, 0, 0, 0))
    for i, (world, label) in enumerate(zip(BUTTON_SOURCES, LEVEL_LABELS)):
        x, y = NORMAL_BUTTONS[world]
        normal = sheet.crop((x, y, x + BUTTON_W, y + BUTTON_H))
        label_button(normal, label, BUTTON_INK[i])
        buttons.paste(normal, (i * BUTTON_W, 0))
        screen.paste(normal, BUTTON_POSITIONS[i])
        # El botón elegido mide 56x31: se repite su última fila para llegar a 32.
        x, y = SELECTED_BUTTONS[world]
        selected = sheet.crop((x, y, x + BUTTON_W, y + BUTTON_H - 1))
        label_button(selected, label, BUTTON_INK[i], selected=True)
        buttons.paste(selected, (i * BUTTON_W, BUTTON_H))
        buttons.paste(selected.crop((0, BUTTON_H - 2, BUTTON_W, BUTTON_H - 1)),
                      (i * BUTTON_W, 2 * BUTTON_H - 1))
    hint_band(screen, "ESPACIO:JUGAR  ESC:MENU")
    screen.convert("RGB").save(IMAGES / "level-select.png")
    buttons.save(IMAGES / "level-buttons.png")


def main():
    menu_cursor()
    info_screen(INSTRUCTIONS, "menu-instructions.png")
    info_screen(CREDITS, "menu-credits.png", spacing=11)
    level_select()
    print("Generadas instrucciones, créditos, selección de nivel y sprites del menú")


if __name__ == "__main__":
    main()
