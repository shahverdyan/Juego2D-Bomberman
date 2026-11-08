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
4. OBJECTS: punto `spawn` del jugador, objetos ocultos y enemigos. Activa el ajuste a cuadrícula
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
Su propiedad entera `itemType` admite 0 para Fire Up, 1 para Bomb Up, 2 para corazón y 3 para Armored Jacket.
Puedes duplicar un punto y cambiar la propiedad para añadir objetos. Solo uno por arbusto.
Al importar se guardan con el nivel.

## Enemigos

En cualquier nivel puedes mover o duplicar los puntos de tipo `enemy` en OBJECTS. Usa `enemyType`
con 0 para Shooter, 1 para Koopa, 2 para Boo y 3 para el jefe. Activa `facingLeft` para que mire a la izquierda.
La casilla de Shooter y Koopa corresponde a sus pies. El Shooter también necesita libre la
casilla superior. El punto de Boo y el del jefe marcan su esquina superior izquierda.
Un nivel con jefe termina solo unos segundos después de vencerlo (sin entrar en la puerta).

Al convertir el mapa, cada enemigo se guarda como `columna fila tipo miraIzquierda` en
`ENEMIES`. La propiedad de mapa `useMapEnemies` conserva esa sección al quitar todos
los enemigos. Ya no hay posiciones fijas en el código: sin esta sección, el nivel no tiene enemigos.

## Aplicar al juego

Guarda el `.tmj` en Tiled (Ctrl+S) y haz doble clic en `ActualizarMapa.cmd`, en la raíz.
El acceso busca Python 3 o el Python incluido en Codex. Después cierra la partida abierta,
compila en Visual Studio y ejecuta. Si prefieres hacerlo desde terminal, con Python 3:

```
python tools/tiled_map.py to-game src/levels/level01.tmj src/levels/level01.txt
```

Para editar otro nivel, sustituye ambos `level01` por su nombre, por ejemplo `level04` para Forest IV.

Si usas el lanzador de Python de Windows, sustituye `python` por `py -3`.
El conversor solo utiliza la biblioteca estándar de Python, sin paquetes adicionales.
Después compila en Visual Studio y ejecuta con Ctrl+F5. La compilación copia el mapa a build.
El juego lee el `.txt`, por eso guardar solo el `.tmj` no cambia aún la partida.
Si prefieres, guarda el editor y pide a Codex que lo importe.

## Editar píxeles de los bloques

La imagen compartida por Tiled y el juego es `src/images/tilesets/world1-atlas.png`.
Mide 64x64 píxeles y contiene 4 columnas por 4 filas de bloques, todos de 16x16.
Abre ese PNG en un editor de píxeles y activa una cuadrícula de 16x16. Usa lápiz
de 1 píxel, sin suavizado; conserva 64x64, las posiciones y los colores que quieras mantener.
La casilla de la esquina superior izquierda es el ID 1 del juego (ID 0 en Tiled).
Cada fila sigue de izquierda a derecha. No añadas espacios entre bloques.

Guarda el PNG y vuelve a abrir el mapa en Tiled para ver la imagen actualizada.
No necesitas recolocar sus casillas: level01.tmj y los mapas que usen ese mismo atlas
comparten los dibujos. Para verlo en el juego, cierra la partida, compila y ejecuta.
Si solo has cambiado píxeles no hace falta ejecutar ActualizarMapa.cmd.

El juego dibuja cada celda de 16x16 a 32x32 unidades y la ventana la amplía a 64x64
píxeles. Un píxel del atlas ocupa 4x4 píxeles de pantalla. La colisión sigue ocupando
la casilla completa y no depende de la transparencia o del dibujo.

Para restaurar los recortes originales corregidos sin cambiar el diseño de ningún mapa:

```
python tools/build_world1.py --atlas-only
```

Esta opción sobrescribe el PNG y su catálogo, así que reemplaza las ediciones manuales
de píxeles. También actualiza la vista general del nivel. El origen correcto de los
recortes de 16x16 en la imagen fuente es (15,12); no se recorta ninguna fila ni se añade
transparencia al pie de la cascada.

Para actualizar el editor desde un `.txt` cambiado fuera de Tiled:

```
python tools/tiled_map.py to-tiled src/levels/level01.txt src/levels/level01.tmj
```

Estas órdenes sobrescriben el destino indicado. `build_world1.py` regenera el diseño base,
por lo que NO debes ejecutarlo después de diseñar tu mapa si quieres conservar tus cambios.

El formato admite una sección ITEMS opcional tras COLLISION; es la que utiliza el conversor
para guardar los puntos. Los mapas antiguos sin esa sección conservan los cuatro objetos originales.

Documentación oficial: https://docs.mapeditor.org/en/stable/reference/json-map-format/
