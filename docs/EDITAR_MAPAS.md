# Editar el nivel gráficamente con Tiled

Descarga Tiled de https://www.mapeditor.org/ (editor independiente, no sustituye nuestro motor).
El archivo preparado es `src/levels/level01.tmj`. Ábrelo en Tiled con Archivo > Abrir.
Ya contiene el atlas y el nivel actual. No hay que recortar imágenes ni escribir filas a mano.

## Pintar el mapa

1. BACKGROUND: fondo, vegetación y cascadas. Usa lápiz, goma, selección y cubo de relleno.
2. FOREGROUND: plataformas, columnas, suelo y puerta visual.
3. COLLISION: activa el ojo para verla. Cualquier casilla pintada aquí bloquea; vacía permite pasar.
   Pinta solo terreno permanente. Arbustos y cascadas adquieren colisión automáticamente.
   Oculta esta capa para ver el aspecto final sin sus marcadores.
4. OBJECTS: punto `spawn` del jugador y puntos de los objetos ocultos. Activa el ajuste a cuadrícula
   y sitúa cada punto en la esquina superior izquierda de su casilla, no en el centro.

El editor usa celdas de 16x16 píxeles del atlas. El juego las dibuja a 32 unidades,
y la cámara muestra 10 columnas x 9 filas (a 64 píxeles por bloque en ventana).
Conserva el mapa ortogonal finito y el tileset World 1. No rotes ni reflejes tiles.

## Cascadas

Coloca una cabecera, debajo tantos tramos centrales como quieras y, al final, el extremo inferior,
todos en la misma columna y en la misma capa. No marques su COLLISION: el juego lo gestiona.
Deja una plataforma lateral a la altura de cabecera + 1 fila para poder colocar una bomba al lado.
La explosión que toca la cabecera retrae la cascada en 1000 ms. Solo quedan cabecera y pie juntos.

| Pieza | ID del atlas en el juego | ID que Tiled muestra (empieza en cero) |
|---|---:|---:|
| Cabecera | 7 | 6 |
| Tramo central | 8 | 7 |
| Pie | 9 | 8 |
| Arbusto | 10 | 9 |

## Objetos ocultos

Cada punto de tipo `item` debe estar sobre un arbusto. Mueve el punto junto con su arbusto.
Su propiedad entera `itemType` indica: 0 Fire Up, 1 Bomb Up, 2 corazón, 3 estrella.
Puedes duplicar un punto y cambiar la propiedad para añadir objetos. Solo uno por arbusto.
Al importar se guardan con el nivel; no hace falta cambiar `Scene::spawnItems`.
Los enemigos todavía conservan los spawns fijos de tu compañero en Scene: no se editan con estos puntos.

## Aplicar al juego

Guarda el `.tmj` en Tiled (Ctrl+S) y haz doble clic en `ActualizarMapa.cmd`, en la raíz.
El acceso busca Python 3 o el Python incluido en Codex. Después cierra la partida abierta,
compila en Visual Studio y ejecuta. Si prefieres hacerlo desde terminal, con Python 3:

```
python tools/tiled_map.py to-game src/levels/level01.tmj src/levels/level01.txt
```

Si usas el lanzador de Python de Windows, sustituye `python` por `py -3`.
El conversor solo utiliza la biblioteca estándar de Python, sin paquetes adicionales.
Después compila en Visual Studio y ejecuta con Ctrl+F5. La compilación copia el mapa a build.
El juego lee el `.txt`, por eso guardar solo el `.tmj` no cambia aún la partida.
Si prefieres, guarda el editor y pide a Codex que lo importe.

Para actualizar el editor desde un `.txt` cambiado fuera de Tiled:

```
python tools/tiled_map.py to-tiled src/levels/level01.txt src/levels/level01.tmj
```

Estas órdenes sobrescriben el destino indicado. `build_world1.py` regenera el diseño base,
por lo que NO debes ejecutarlo después de diseñar tu mapa si quieres conservar tus cambios.

El formato admite una sección ITEMS opcional tras COLLISION; es la que utiliza el conversor
para guardar los puntos. Los mapas antiguos sin esa sección conservan los cuatro objetos originales.

Documentación oficial: https://docs.mapeditor.org/en/stable/reference/json-map-format/
