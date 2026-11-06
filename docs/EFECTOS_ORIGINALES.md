# Gráficos originales: bombas, explosiones y objetos

Fuente: `src/images/Game Boy _ GBC - Pocket Bomberman - Characters - Overworld (Color).gif`.
Generación: `python tools/build_original_effects.py` (Pillow). Los recortes se documentan
con coordenadas en `src/images/original-effects.json`. La imagen fuente se conserva.

## Bomba

Se usan los tres sprites de la cuarta fila (4.º, 5.º y 6.º), en una celda común de 20x20.
El cuerpo mantiene centro y base; las chispas conservan espacio transparente alrededor.
Se conservan los píxeles originales, sin escalado artificial de un único dibujo.
Pulso pequeño/medio/grande/medio, con aceleración al acercarse la detonación.
La mecha dura 2 s. Cada bomba calcula su fase por su propio tiempo.

## Explosión

Las tres filas siguientes contienen centros 16x16, extremos y tramos de unión.
Se ensamblan en casillas 16x16 sin estirarlos: se repiten las medias piezas de unión
para cubrir exactamente la longitud de cada bloque del alcance.
El atlas resultante tiene siete columnas (centro, arriba, abajo, izquierda, derecha,
vertical y horizontal) y tres filas de intensidad. Crece y decrece durante los 0.4 s
que ya dura el daño. Bombas distintas conservan tiempos de animación independientes.

## Objetos

Se toman de la primera fila de recuadros dorados (fila 8 indicada por el usuario):

- Bomb Up: primero, coordenadas 21,117 (16x16).
- Fire Up: segundo, coordenadas 39,117 (16x16).
- Invulnerabilidad: cuarto, coordenadas 75,117 (16x16).
- Corazón dorado: quinto, coordenadas 93,117 (16x16).

El atlas del juego mantiene el orden del enum ItemType: Fire Up, Bomb Up, corazón,
invulnerabilidad. Sus efectos no cambian. Los scripts de HUD y objetos ya no se pisan
entre sí al regenerar los recursos.

## Cascada

El pie se recorta completo a 16x16 desde (127,124), con su última fila original opaca.
El origen de la cuadrícula del escenario es (15,12); el desfase anterior de una fila
se corrige en todos los bloques. El atlas mantiene los mismos IDs y dimensiones.
build_original_effects.py solo genera bombas, fuego y objetos; no sobrescribe el terreno.

## Verificación

`tests/original_effects_test.py`: ancla fija de las bombas, continuidad entre piezas,
centros y objetos originales y recorte completo del pie de la cascada.
`tests/bomb_test.cpp`: secuencia temporal, colisiones y colocación conservadas.
`--original-effects-test`: capturas ocultas de tres tamaños de bomba, los estados de la
explosión y los cuatro objetos. `--map-effects-test`: revisión del pie durante su apertura.
