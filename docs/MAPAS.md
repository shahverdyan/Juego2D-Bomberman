# Forest III

El mapa de cascadas ocupa 34 × 19 casillas. Abre `src/levels/level03.tmj` en Tiled.
Rick empieza en fila 16, columna 6. La puerta está en fila 3, columna 5.
La vista general está en `docs/level03-preview.png`.

# Forest V: cámara y obstáculos

Para editar Forest V gráficamente, abre `src/levels/level05.tmj` con Tiled y lee EDITAR_MAPAS.md.
La cámara enseña exactamente 10x9 bloques del mundo; el HUD ocupa su propia franja inferior.
Ventana 640x608: área de juego 640x576 (bloques de 64 px) + HUD inferior de 32 px.

Las cascadas son sólidas. Una explosión sobre la cabecera inicia una retracción de 1 s:
el pie sube de forma continua y desaparecen los segmentos inferiores; quedan dos bloques sólidos.
No hace falta tocar la cabecera con el cuerpo de la bomba: la activa su explosión.

| Cascada | Cabecera (columna,fila) | Bomba al lado (columna,fila) |
|---|---|---|
| Entrada | 6,22 | 5,22 |
| Central | 27,12 | 28,12 |
| Este | 40,11 | 41,11 |

Coordenadas desde cero. Hay plataformas de apoyo en las casillas de bomba.
Los arbustos son sólidos y destructibles: el fuego incluye el primer arbusto y se detiene allí.
Destruirlo libera su colisión y revela el objeto oculto que configuró el compañero.
Los cuatro objetos iniciales permanecen en 3,24; 4,22; 11,20; 15,16.

El formato TILEMAP2 y las dimensiones 48x26 se conservan. Añadida sección opcional ITEMS,
con filas `columna fila tipo`, donde tipo es 0=Fire Up, 1=Bomb Up, 2=corazón, 3=estrella.
El .tmj lleva capas BACKGROUND, FOREGROUND, COLLISION y OBJECTS.

# Referencia del diseño anterior

El mapa de Forest V es `src/levels/level05.txt`. Conserva 48 x 26 celdas, pero el diseño
es asimétrico, con formas huecas inspiradas en las estructuras del original:

- Zona oeste: entrada fácil, arco de troncos, saltos interiores alternados y una repisa lateral.
- Zona central: gran cámara bajo una cascada, plataformas dentro y ruta sobre el techo.
- Zona este: ascenso en zigzag dentro de otro arco, hasta el marcador de salida (43,7).
- Desvío superior: desde el techo central se asciende a una copa alta y se vuelve hacia el este.
- Paso inferior continuo: permite recuperar la ruta después de caer. No hay pozos mortales.

La imagen general está en `docs/level05-preview.png`. La puerta usa el marcador (43,7) y lleva a la sala del jefe (level06).
La distribución se puede recorrer con las físicas actuales, sin depender de bombas aún no implementadas.

## Gráficos originales

- Imagen suministrada: `src/images/tilesets/Game Boy _ GBC - Pocket Bomberman - Stages (World 1) - Stage 1-4.png`.
- Atlas extraído: `src/images/tilesets/world1-atlas.png`, 64 x 64 píxeles.
- Catálogo y coordenadas originales: `src/images/tilesets/world1-atlas.json`.
- Se conservan exactamente los píxeles de 16 tiles de 16 x 16; se dibujan a 32 x 32 con filtro nearest.
- Se excluyen la firma y la ilustración de la parte inferior de la imagen. Se conservan en el original.
- El rip suministrado está firmado por Fret & Woppet; juego de Hudson Soft. Mantener estos créditos en la entrega.
- El personaje sigue siendo el provisional de Bubble. Las cascadas ahora tienen colisión y animación de apertura.
- El tile 16 (`door_marker`) marca la casilla inferior izquierda de la puerta-portal (2x2 tiles).
  El juego no lo dibuja: coloca ahí el portal, que se abre al no quedar enemigos.

## Formato TILEMAP2

```
TILEMAP2
48 26
32 32
images/tilesets/world1-atlas.png
4 4
SPAWN 2 24
BACKGROUND
...26 filas de 48 enteros separados por espacios...
FOREGROUND
...26 filas de 48 enteros separados por espacios...
COLLISION
...26 filas de 48 caracteres 0 o 1, sin espacios...
```

Las dimensiones del mapa y del atlas van en columnas/filas. SPAWN es la celda inicial
IDs gráficos: 0 vacío; 1 a 16 según
el catálogo, por filas del atlas. COLLISION decide qué bloquea al jugador de forma
independiente del aspecto: 1 sólido, 0 atravesable. El exterior del mapa es sólido.
El follaje decorativo (tile 10) puede desaparecer al recibir una explosión; esto no
cambia la colisión del nivel. Si estaba en BACKGROUND se restaura el tile base 1; si
estaba en FOREGROUND se deja vacío, mostrando el fondo de la capa inferior.

El cargador valida dimensiones, filas, IDs y que el punto de aparición esté libre.
Dibuja BACKGROUND antes de FOREGROUND. Las explosiones pueden eliminar el tile 10 de follaje;
los bloques sólidos aún no son destructibles. No hay importación de Tiled.

Para modificar el diseño a mano, edita las capas en `src/levels/level05.txt` y compila.
Para regenerar el diseño base y el atlas con Python + Pillow:

```
python tools/build_world1.py --layout-only
```

Con `--layout-only`, la orden SOBRESCRIBE el mapa de Forest V y su vista general, reutilizando
el atlas existente. Sin esa opción, también regenera el atlas y el catálogo. No la
uses después de editar el mapa a mano salvo que quieras regenerarlo. La imagen fuente
no se modifica. Las coordenadas de extracción y las columnas/plataformas están en el script.

## Comprobaciones

`tests/level_test.cpp` comprueba el cargador, errores de entrada, separación entre
decoración y colisión y rutas de salto usando la física real del jugador: inicio,
arco oeste, interior central, ascenso este, repisa lateral, copa superior y puerta.
La prueba incluye caminar fuera de una plataforma y caer, y comprueba rutas de vuelta
al inicio para todas las posiciones alcanzadas en el muestreo (cada cuatro píxeles). También se han
comparado los 16 recortes píxel a píxel con su región de la imagen original.

Controles: flechas izquierda/derecha o A/D para moverse; espacio coloca una bomba; arriba/W salta; R vuelve al inicio.
