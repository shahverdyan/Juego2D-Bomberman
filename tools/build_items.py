"""Generate the original framed Fire Up, Bomb Up, golden heart and invulnerability item."""
from PIL import Image
from build_original_effects import SOURCE, build_items
if __name__=='__main__':
    build_items(Image.open(SOURCE).convert('RGBA'))
    print('Generated original Pocket Bomberman item atlas.')
