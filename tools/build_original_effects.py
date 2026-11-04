"""Extract Pocket Bomberman bomb/fire sprites and power-ups, retaining source pixels.
Fire half-segments are joined to fill the game's 16x16 blast cells without stretching.
"""
from pathlib import Path
import json
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
IMAGES = ROOT / 'src/images'
SOURCE = IMAGES / 'Game Boy _ GBC - Pocket Bomberman - Characters - Overworld (Color).gif'
BODY = (64,40,24,255)
BOMB_BOXES = [(57,51,73,67),(75,51,91,67),(92,51,108,67)]
# Indexed in the same order as ItemType: Fire Up, Bomb Up, Heart, Star.
ITEM_BOXES = [(39,117,55,133),(21,117,37,133),(93,117,109,133),(75,117,91,133)]


def build_bombs(source):
    atlas = Image.new('RGBA',(60,20))
    for frame,box in enumerate(BOMB_BOXES):
        cut = source.crop(box)
        # Align the lower bomb body, independently of flickering fuse embers.
        body_x = [x for y in range(8,16) for x in range(16) if cut.getpixel((x,y)) == BODY]
        center = (min(body_x)+max(body_x))/2
        dx = round(9.5-center)
        atlas.alpha_composite(cut,(frame*20+dx,3))
    atlas.save(IMAGES/'original-bomb-atlas.png')


def fire_cells(source,frame):
    x=47+18*frame
    horizontal=source.crop((17+10*frame,71,25+10*frame,87)) # 8x16
    vertical=source.crop((x,79,x+16,87))                    # 16x8
    top=source.crop((x,70,x+16,77))                        # 16x7 (no blank seam row)
    bottom=source.crop((x,107,x+16,115))                   # 16x8
    left=source.crop((17+10*frame,89,25+10*frame,105))
    right=source.crop((101+10*frame,89,109+10*frame,105))
    def cell(*parts):
        result=Image.new('RGBA',(16,16))
        for image,pos in parts: result.alpha_composite(image,pos)
        return result
    # CENTER, TOP_END, BOTTOM_END, LEFT_END, RIGHT_END, VERTICAL_MID, HORIZONTAL_MID
    return [source.crop((x,89,x+16,105)),
            cell((top,(0,1)),(vertical,(0,8))),
            cell((vertical,(0,0)),(bottom,(0,8))),
            cell((left,(0,0)),(horizontal,(8,0))),
            cell((horizontal,(0,0)),(right,(8,0))),
            cell((vertical,(0,0)),(vertical,(0,8))),
            cell((horizontal,(0,0)),(horizontal,(8,0)))]


def build_fire(source):
    atlas=Image.new('RGBA',(7*16,3*16))
    preview=Image.new('RGBA',(3*48,48))
    for frame in range(3):
        cells=fire_cells(source,frame)
        for kind,cell in enumerate(cells): atlas.alpha_composite(cell,(kind*16,frame*16))
        for kind,pos in [(0,(16,16)),(1,(16,0)),(2,(16,32)),(3,(0,16)),(4,(32,16))]:
            preview.alpha_composite(cells[kind],(frame*48+pos[0],pos[1]))
    atlas.save(IMAGES/'original-fire-atlas.png')
    preview.resize((preview.width*4,preview.height*4),Image.Resampling.NEAREST).save(ROOT/'docs/original-fire-preview.png')


def build_items(source):
    atlas=Image.new('RGBA',(128,32))
    for index,box in enumerate(ITEM_BOXES):
        tile=source.crop(box).resize((32,32),Image.Resampling.NEAREST)
        atlas.alpha_composite(tile,(index*32,0))
    atlas.save(IMAGES/'items-atlas.png')


def main():
    source=Image.open(SOURCE).convert('RGBA')
    build_bombs(source)
    build_fire(source)
    build_items(source)
    manifest=dict(source=SOURCE.name,bomb_cells=[20,20],bomb_source_boxes=BOMB_BOXES,
                  bomb_anchor=dict(center_x=10,body_bottom=19),fire_cells=[16,16],
                  fire_columns=['center','top','bottom','left','right','vertical','horizontal'],
                  fire_frame_x=[47,65,83],item_source_boxes=ITEM_BOXES,
                  item_order=['fire_up','bomb_up','gold_heart','star'])
    (IMAGES/'original-effects.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
    print('Extracted 3 bomb frames, 3x7 fire pieces and 4 original power-ups.')


if __name__=='__main__': main()
