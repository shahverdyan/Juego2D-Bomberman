# Nivel 1: mundo del bosque

El mapa activo es `src/levels/level01.txt`. Conserva 48 x 26 celdas, pero el diseño
es asimétrico, con formas huecas inspiradas en las estructuras del original:

- Zona oeste: entrada fácil, arco de troncos, saltos interiores alternados y una repisa lateral.
- Zona central: gran cámara bajo una cascada, plataformas dentro y ruta sobre el techo.
- Zona este: ascenso en zigzag dentro de otro arco, hasta el marcador de salida (43,7).
- Desvío superior: desde el techo central se asciende a una copa alta y se vuelve hacia el este.
- Paso inferior continuo: permite recuperar la ruta después de caer. No hay pozos mortales.

La imagen general está en `docs/level01-preview.png`. La puerta sigue siendo visual.
La distribución se puede recorrer con las físicas actuales, sin depender de bombas aún no implementadas.

## Gráficos originales

- Imagen suministrada: `src/images/tilesets/Game Boy _ GBC - Pocket Bomberman - Stages (World 1) - Stage 1-4.png`.
- Atlas extraído: `src/images/tilesets/world1-atlas.png`, 64 x 64 píxeles.
- Catálogo y coordenadas originales: `src/images/tilesets/world1-atlas.json`.
- Se conservan exactamente los píxeles de 16 tiles de 16 x 16; se dibujan a 32 x 32 con filtro nearest.
- Se excluyen la firma y la ilustración de la parte inferior de la imagen. Se conservan en el original.
- El rip suministrado está firmado por Fret & Woppet; juego de Hudson Soft. Mantener estos créditos en la entrega.
- El personaje sigue siendo el provisional de Bubble. Las cascadas son estáticas y decorativas.
- La puerta del recorrido superior es un marcador gráfico; todavía no cambia de nivel.

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
de la esquina superior izquierda del jugador. IDs gráficos: 0 vacío; 1 a 16 según
el catálogo, por filas del atlas. COLLISION decide qué bloquea al jugador de forma
independiente del aspecto: 1 sólido, 0 atravesable. El exterior del mapa es sólido.

El cargador valida dimensiones, filas, IDs y que el punto de aparición esté libre.
Dibuja BACKGROUND antes de FOREGROUND. No hay aún tiles destructibles ni importación de Tiled.

Para modificar el diseño a mano, edita las capas en `src/levels/level01.txt` y compila.
Para regenerar el diseño base y el atlas con Python + Pillow:

```
python tools/build_world1.py --layout-only
```

Con `--layout-only`, la orden SOBRESCRIBE `level01.txt` y la vista general, reutilizando
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

Controles: flechas o A/D, espacio/arriba/W para saltar, R para volver al inicio.
