import copy
import importlib.util
import json
from pathlib import Path
from tempfile import TemporaryDirectory

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('tiled_map', ROOT/'tools/tiled_map.py')
converter = importlib.util.module_from_spec(spec)
spec.loader.exec_module(converter)

with TemporaryDirectory(dir=ROOT/"build") as folder:
    folder = Path(folder)
    editor, game = folder/'level.tmj', folder/'level.txt'
    for number in (1, 2, 3, 5):
        source = ROOT/f'src/levels/level{number:02d}.txt'
        converter.export_tiled(source, editor)
        converter.import_tiled(editor, game)
        assert converter.read_level(game) == converter.read_level(source)

    # Las coordenadas de esta prueba pertenecen a Forest V.
    data = json.loads(editor.read_text())
    layers = {layer['name']:layer for layer in data['layers']}
    # Move the first hidden power-up together with its bush, as in the editor.
    w = data['width']
    layers['BACKGROUND']['data'][24*w+3] = 1
    layers['BACKGROUND']['data'][24*w+5] = 10
    layers['OBJECTS']['objects'][1]['x'] = 5*16
    editor.write_text(json.dumps(data))
    converter.import_tiled(editor,game)
    moved=converter.read_level(game)
    assert moved['items'][0] == (5,24,0) and moved['layers']['BACKGROUND'][24*w+5]==10

    for invalid in ['flipped','item_without_bush','spawn_in_bush']:
        bad=copy.deepcopy(data)
        if invalid=='flipped': bad['layers'][0]['data'][0]=0x80000001
        elif invalid=='item_without_bush': bad['layers'][3]['objects'][1]['x']=8*16
        else: bad['layers'][3]['objects'][0]['x']=5*16
        editor.write_text(json.dumps(bad))
        try:
            converter.import_tiled(editor,game)
        except ValueError:
            pass
        else:
            raise AssertionError('Invalid editor data accepted: '+invalid)
print('PASS: Tiled round trip, moved bush/power-up, rejected unsupported or invalid edits')
