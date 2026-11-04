"""Extract the original 8x8 HUD icons; reproduce the numeric style from the reference.
No resampling or repainting of icon pixels: only the pink backdrop becomes transparent.
"""
from pathlib import Path
import json
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
IMAGES = ROOT / 'src/images'
SOURCE = IMAGES / 'Game Boy _ GBC - Pocket Bomberman - Characters - Overworld (Color).gif'
PINK = (236,149,205,255)
INK = (0,40,24,255)
SOURCE_PINK = (248,112,208)
GLYPHS = {
    '0':['........','..#####.','.##...##','.##...##','.##...##','.##...##','.##...##','..#####.'],
    '1':['........','....##..','...###..','...###..','....##..','....##..','....##..','...####.'],
    '2':['........','..#####.','.##...##','.##...##','....###.','..##....','.##.....','.#######'],
    '3':['........','..#####.','.##...##','.##...##','....###.','.##...##','.##...##','..#####.'],
    '4':['........','....###.','...####.','..##.##.','.##..##.','.#######','.....##.','.....##.'],
    '5':['........','.#######','.##.....','.######.','.##...##','......##','.##...##','..#####.'],
    '6':['........','..#####.','.##...##','.##.....','.######.','.##...##','.##...##','..#####.'],
    '7':['........','.#######','.....##.','....##..','...##...','..##....','..##....','..##....'],
    '8':['........','..#####.','.##...##','.##...##','..#####.','.##...##','.##...##','..#####.'],
    '9':['........','..#####.','.##...##','.##...##','..######','......##','.##...##','..#####.'],
    ':':['........','...##...','...##...','........','........','...##...','...##...','........'],
}
# Atlas IDs 11..16; third row in the supplied sheet, source pixel coordinates.
ICONS = [('clock',55),('face',25),('enemies',35),('bomb',15),('range',5),('gold_heart',45)]


def extract_icon(source, x):
    icon = source.crop((x,42,x+8,50))
    for py in range(8):
        for px in range(8):
            if icon.getpixel((px,py))[:3] == SOURCE_PINK:
                icon.putpixel((px,py),(0,0,0,0))
    return icon


def main():
    source = Image.open(SOURCE).convert('RGBA')
    atlas = Image.new('RGBA',(18*8,8),(0,0,0,0))
    for index, rows in enumerate(GLYPHS.values()):
        for y, row in enumerate(rows):
            for x, value in enumerate(row):
                if value == '#': atlas.putpixel((index*8+x,y),INK)
    for index, (_,x) in enumerate(ICONS,11):
        atlas.alpha_composite(extract_icon(source,x),(index*8,0))
    atlas.paste(PINK,(17*8,0,18*8,8))
    atlas.save(IMAGES/'hud-original-atlas.png')
    heart = extract_icon(source,45)
    heart.save(IMAGES/'hud-gold-heart.png')
    manifest = dict(source=SOURCE.name, cell_size=8, pink=list(PINK),
                    glyphs='0123456789:', icons=[dict(id=i,name=name,rect=[x,42,8,8])
                    for i,(name,x) in enumerate(ICONS,11)], panel_id=17)
    (IMAGES/'hud-original-atlas.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
    # A reference layout for asset inspection, in native 160x8 HUD coordinates.
    preview = Image.new('RGBA',(160,8),PINK)
    def draw(index,x): preview.alpha_composite(atlas.crop((index*8,0,index*8+8,8)),(x,0))
    draw(11,8)
    for x,c in zip([16,24,32,40],'4:00'): draw(list(GLYPHS).index(c),x)
    for i,x in [(12,64),(2,72),(16,56),(13,88),(4,96),(14,112),(1,120),(15,136),(1,144)]: draw(i,x)
    preview.resize((640,32),Image.Resampling.NEAREST).save(ROOT/'docs/hud-preview.png')
    print('Extracted original HUD icons; generated number atlas and golden heart HUD icon.')


if __name__ == '__main__':
    main()
