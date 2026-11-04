from pathlib import Path
import importlib.util
from PIL import Image

ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('original_effects',ROOT/'tools/build_original_effects.py')
effects=importlib.util.module_from_spec(spec)
spec.loader.exec_module(effects)
def visible(image):
    result=Image.new('RGBA',image.size)
    result.alpha_composite(image)
    return result.tobytes()

source=Image.open(effects.SOURCE).convert('RGBA')
bombs=Image.open(ROOT/'src/images/original-bomb-atlas.png').convert('RGBA')
assert bombs.size==(60,20)
for frame in range(3):
    points=[(x,y) for y in range(11,20) for x in range(20)
            if bombs.getpixel((frame*20+x,y))==effects.BODY]
    assert (min(x for x,y in points)+max(x for x,y in points))/2==9.5
    assert max(y for x,y in points)==18

fire=Image.open(ROOT/'src/images/original-fire-atlas.png').convert('RGBA')
assert fire.size==(112,48)
for frame in range(3):
    cells=effects.fire_cells(source,frame)
    # Pieces stay connected both to the centre and to another middle segment.
    for first,second,axis in [(cells[3],cells[0],'x'),(cells[0],cells[4],'x'),
                              (cells[1],cells[0],'y'),(cells[0],cells[2],'y'),
                              (cells[5],cells[5],'y'),(cells[6],cells[6],'x')]:
        assert any(first.getpixel((15,i) if axis=='x' else (i,15))[3] and
                   second.getpixel((0,i) if axis=='x' else (i,0))[3] for i in range(16))
    x=47+18*frame
    assert visible(fire.crop((0,frame*16,16,frame*16+16)))==visible(source.crop((x,89,x+16,105)))

items=Image.open(ROOT/'src/images/items-atlas.png').convert('RGBA')
for i,box in enumerate(effects.ITEM_BOXES):
    assert visible(items.crop((i*32,0,i*32+32,32)))==visible(source.crop(box).resize((32,32),Image.Resampling.NEAREST))

tiles=Image.open(ROOT/'src/images/tilesets/world1-atlas.png').convert('RGBA')
foot=tiles.crop((0,32,16,48))
assert all(foot.getpixel((x,15))[3]==255 for x in range(16))
terrain_source=Image.open(ROOT/'src/images/tilesets/Game Boy _ GBC - Pocket Bomberman - Stages (World 1) - Stage 1-4.png').convert('RGBA')
assert visible(foot)==visible(terrain_source.crop((127,124,143,140)))
print('PASS: fixed bomb anchors, connected fire pieces, requested original item crops and complete 16x16 waterfall footer')
