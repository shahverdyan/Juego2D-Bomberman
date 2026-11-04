"""Convert our finite TILEMAP2 levels to/from Tiled JSON (.tmj), standard library only."""
import argparse
import json
import os
from pathlib import Path

DEFAULT_ITEMS = [(3, 24, 0), (4, 22, 1), (11, 20, 2), (15, 16, 3)]


def read_level(path):
    lines = Path(path).read_text(encoding='utf-8-sig').splitlines()
    if lines[0] != 'TILEMAP2':
        raise ValueError('Expected TILEMAP2')
    w, h = map(int, lines[1].split())
    size, block = map(int, lines[2].split())
    cols, rows = map(int, lines[4].split())
    spawn = list(map(int, lines[5].split()[1:]))
    layers = {}
    cursor = 6
    for name in ['BACKGROUND', 'FOREGROUND', 'COLLISION']:
        if lines[cursor] != name:
            raise ValueError('Missing ' + name)
        cursor += 1
        cells = []
        for line in lines[cursor:cursor+h]:
            row = list(map(int, line)) if name == 'COLLISION' else list(map(int, line.split()))
            if len(row) != w:
                raise ValueError('Invalid row width')
            cells.extend(row)
        layers[name] = cells
        cursor += h
    items = DEFAULT_ITEMS
    if cursor < len(lines):
        tag, count = lines[cursor].split()
        if tag != 'ITEMS':
            raise ValueError('Unknown optional section')
        items = [tuple(map(int, line.split())) for line in lines[cursor+1:cursor+1+int(count)]]
    return dict(width=w, height=h, size=size, block=block, atlas=lines[3],
                columns=cols, rows=rows, spawn=spawn, layers=layers, items=items)


def export_tiled(source, target):
    source, target = Path(source).resolve(), Path(target).resolve()
    level = read_level(source)
    w, h = level['width'], level['height']
    # Source atlas cells are 16px. The game draws them at 32 world units.
    tile = 16
    image = os.path.relpath(source.parent.parent / level['atlas'], target.parent).replace('\\', '/')
    layers = []
    for i, name in enumerate(['BACKGROUND', 'FOREGROUND', 'COLLISION']):
        cells = level['layers'][name]
        layers.append(dict(id=i+1, name=name, type='tilelayer', x=0, y=0,
                           width=w, height=h, visible=name != 'COLLISION', opacity=1,
                           data=[6 if c else 0 for c in cells] if name == 'COLLISION' else cells))
    objects = [dict(id=1, name='spawn', type='player', point=True,
                    x=level['spawn'][0]*tile, y=level['spawn'][1]*tile, rotation=0)]
    for x, y, item_type in level['items']:
        objects.append(dict(id=len(objects)+1, name=['Fire Up','Bomb Up','Heart','Star'][item_type],
                            type='item', point=True, x=x*tile, y=y*tile, rotation=0,
                            properties=[dict(name='itemType', type='int', value=item_type)]))
    layers.append(dict(id=4, name='OBJECTS', type='objectgroup', visible=True, opacity=1,
                       draworder='topdown', objects=objects))
    data = dict(type='map', version='1.10', orientation='orthogonal', renderorder='right-down',
                infinite=False, width=w, height=h, tilewidth=tile, tileheight=tile,
                nextlayerid=5, nextobjectid=len(objects)+1, layers=layers,
                properties=[dict(name='gameTileSize', type='int', value=level['size']),
                            dict(name='gameAtlas', type='string', value=level['atlas'])],
                tilesets=[dict(firstgid=1, name='World 1', image=image,
                               imagewidth=level['columns']*tile, imageheight=level['rows']*tile,
                               tilewidth=tile, tileheight=tile, columns=level['columns'],
                               tilecount=level['columns']*level['rows'], margin=0, spacing=0)])
    target.write_text(json.dumps(data, indent=2)+'\n', encoding='utf-8')


def properties(obj):
    return {p['name']: p['value'] for p in obj.get('properties', [])}


def import_tiled(source, target):
    data = json.loads(Path(source).read_text(encoding='utf-8-sig'))
    if data.get('infinite') or data.get('orientation') != 'orthogonal':
        raise ValueError('Use a finite orthogonal map')
    w, h, tile = data['width'], data['height'], data['tilewidth']
    if tile != 16 or data['tileheight'] != 16 or not (3 <= w <= 512 and 3 <= h <= 512):
        raise ValueError('Use 16x16 tiles, dimensions from 3 to 512')
    if len(data['tilesets']) != 1:
        raise ValueError('Use the supplied World 1 tileset only')
    tileset = data['tilesets'][0]
    # Tiled may save a tileset separately when the user edits its properties.
    firstgid = tileset['firstgid']
    if 'source' in tileset:
        tileset = json.loads((Path(source).parent / tileset['source']).read_text(encoding='utf-8-sig'))
    cols = tileset['columns']
    rows = tileset['tilecount'] // cols
    if (cols, rows) != (4, 4):
        raise ValueError('Expected the 4x4 World 1 atlas')
    layers = {layer['name']: layer for layer in data['layers']}
    cells = {}
    for name in ['BACKGROUND', 'FOREGROUND', 'COLLISION']:
        layer = layers[name]
        if (layer.get('offsetx',0) or layer.get('offsety',0) or layer.get('x',0) or layer.get('y',0)):
            raise ValueError('Layers must remain aligned to the map origin')
        raw = layer['data']
        if not isinstance(raw, list) or len(raw) != w*h:
            raise ValueError('Use uncompressed CSV/JSON tile arrays')
        if any(not isinstance(gid, int) or gid < 0 or gid & 0xf0000000 for gid in raw):
            raise ValueError('Flipped or rotated tiles are not supported; use ordinary tiles')
        if any(gid and not firstgid <= gid < firstgid+cols*rows for gid in raw):
            raise ValueError('Unknown tile ID')
        cells[name] = [int(bool(gid)) if name == 'COLLISION' else gid-firstgid+1 if gid else 0 for gid in raw]
    objects = layers.get('OBJECTS', {}).get('objects', [])
    spawn = None
    items = []
    for obj in objects:
        kind = obj.get('type', obj.get('class',''))
        if kind not in ['player', 'item']:
            continue
        x, y = obj['x']/tile, obj['y']/tile
        if x != int(x) or y != int(y) or not (0 <= x < w and 0 <= y < h):
            raise ValueError('Place point objects at tile corners with grid snapping')
        x, y = int(x), int(y)
        if kind == 'player':
            if spawn is not None:
                raise ValueError('Keep exactly one player spawn')
            spawn = [x,y]
        else:
            item_type = properties(obj).get('itemType')
            if item_type not in range(4):
                raise ValueError('itemType must be 0=Fire Up, 1=Bomb Up, 2=Heart, 3=Star')
            if not any(cells[n][y*w+x] == 10 for n in ['BACKGROUND','FOREGROUND']):
                raise ValueError('Every hidden item must be placed on a bush (tile 10)')
            if any(ix == x and iy == y for ix,iy,_ in items):
                raise ValueError('Only one hidden item per bush')
            items.append((x,y,item_type))
    if spawn is None:
        raise ValueError('Keep one player point in OBJECTS')
    i = spawn[1]*w+spawn[0]
    if cells['COLLISION'][i] or any(cells[n][i] in [7,8,9,10] for n in ['BACKGROUND','FOREGROUND']):
        raise ValueError('Spawn must be empty')
    props = properties(data)
    size = props.get('gameTileSize',32)
    if size != 32:
        raise ValueError('Keep gameTileSize=32')
    lines = ['TILEMAP2', f'{w} {h}', f'{size} {size}', props.get('gameAtlas','images/tilesets/world1-atlas.png'),
             f'{cols} {rows}', f'SPAWN {spawn[0]} {spawn[1]}']
    for name in ['BACKGROUND','FOREGROUND','COLLISION']:
        lines.append(name)
        for y in range(h):
            row = cells[name][y*w:(y+1)*w]
            lines.append(''.join(map(str,row)) if name == 'COLLISION' else ' '.join(map(str,row)))
    lines.append(f'ITEMS {len(items)}')
    lines.extend(f'{x} {y} {kind}' for x,y,kind in items)
    Path(target).write_text('\n'.join(lines)+'\n', encoding='utf-8')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('direction', choices=['to-tiled','to-game'])
    parser.add_argument('source', type=Path)
    parser.add_argument('target', type=Path)
    args = parser.parse_args()
    try:
        (export_tiled if args.direction == 'to-tiled' else import_tiled)(args.source,args.target)
    except (ValueError, KeyError, OSError) as error:
        parser.exit(1, f'Conversion error: {error}\n')
    print(f'Saved {args.target}')
