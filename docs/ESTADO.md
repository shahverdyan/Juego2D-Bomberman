# Estado (3 noviembre 2026)

## Preparación

Proyecto movido desde 02-Bubble, sin duplicar las bibliotecas. Solución:
`Bomberman.sln`, proyecto `src/Bomberman.vcxproj`, dependencias `libs/`.
PDF movidos a `docs/`. Debug y Release: v145, SDK 10.0, Win32.
Compilación copia recursos y glew32.dll al directorio del ejecutable.
La carpeta actual es `C:/Users/alexs/Desktop/Bomberman`, movida por el usuario.
No se ha encontrado `.git` en la nueva ubicación; no se ha creado otro repositorio.

Los otros ejemplos se retiraron tras guardar `plantilla-antes-fase1.zip` en
`C:/Users/alexs/.codex/visualizations/2026/10/17/01a14a71-fecc-7e31-bda5-39840012f8f3/`.
La carpeta antigua `../OpenGL_VS2015/2DGame/02-Bubble/.vs` no pudo borrarse porque
Visual Studio la mantiene abierta. Solo queda caché, no es una dependencia.

## Fase 1: base y terreno original integrados

- Movimiento 160 unidades/s, salto inicial -440 unidades/s, gravedad 1100 unidades/s².
- Simulación fija cada 8 ms, independiente del render. Pausas mayores a 250 ms se limitan.
- Colisiones en cuatro direcciones, subpasos para evitar atravesar tiles y límites del mapa sólidos.
- Saltar requiere nueva pulsación, sin salto automático al mantener la tecla.
- Cámara centrada y limitada al mapa. R devuelve al inicio.
- Escenario de 48x26 tiles de 32 unidades con arte original del mundo 1.
- Limpieza de recursos antes de cerrar OpenGL; validación básica del archivo de nivel.
- Corregidos liberación de memoria de SOIL y enlace de salida de fragment shader heredados.

El terreno ya usa tiles originales extraídos de la imagen suministrada. El personaje
sigue siendo Bubble. El mapa nuevo mide 48x26 celdas, con columnas y plataformas
propias; ver MAPAS.md y level01-preview.png. El diseño se revisó el 2 de noviembre. El formato TILEMAP2 separa fondo,
primer plano y colisiones y define el spawn. El script tools/build_world1.py
reproduce la extracción sin modificar la fuente y genera el diseño inicial.
Las cascadas y el marcador de puerta son decorativos, todavía sin animación ni lógica.

## Fase 2: lógica inicial de bombas (3 noviembre)

- Espacio coloca una bomba alineada a la celda del jugador; ↑/W conservan el salto.
- La bomba no tiene gravedad; su mecha dura 3 s. Tres fotogramas de fuego avanzan lentamente durante la mecha; en los últimos 0.5 s la bomba aumenta de tamaño y el fuego se detiene al detonar. La cruz permanece 0.5 s y luego desaparece.
- El alcance inicial es 1 celda en cruz, se detiene ante terreno sólido y destruye el follaje decorativo (tile 10); no modifica la colisión.
- El jugador puede atravesar la bomba desde abajo/lateralmente y aterrizar sobre ella para saltar.
- La bomba usa su sprite propio con el fuego animado encima mientras espera; la cruz de la explosión usa el mismo fotograma congelado al detonar. Se usan solo tres fotogramas consecutivos del atlas. La bomba se dibuja un 25% más grande durante los últimos 0.5 s. El atlas extraído del spritesheet original conserva la transparencia.
- El análisis del spritesheet confirma que la región de fuego ocupa una cuadrícula 9x7 de 16x16 píxeles, con la animación horizontal avanzando a derecha y la secuencia de 4 fotogramas tomando las columnas 0..3 de la fila superior. El centro, los extremos y los segmentos intermedios de la cruz se pueden derivar de esa misma matriz para ampliarla con piezas específicas en futuras iteraciones.
- Al destruir follaje de BACKGROUND se restaura el tile base 1; en FOREGROUND se deja vacío. La colisión no cambia.
- La capacidad (inicia en 1) y el alcance (inicia en 1 tile) son atributos independientes, ampliables mediante futuros power-ups.
- No se permite más de una bomba en la misma celda. R reinicia al jugador y limpia bombas activas.
- Pendiente: daño al jugador/enemigos, bloques destructibles y recoger power-ups (fase 2).

## Enemigo Shooter (3 noviembre)

- Nuevas clases `Shooter.h/.cpp`. Cuerpo de 32x48 con la misma física de `Movement.h` (gravedad, colisión), sin salto.
- Patrulla a 60 unidades/s y se da la vuelta ante una pared o el borde de la plataforma.
- Ve al jugador solo si está delante, a 8 tiles como máximo, en la línea de la bala y sin tiles sólidos en medio. Entonces se para.
- Dispara 400 ms después de verlo y luego cada 1200 ms. Bala horizontal de 16x16 a 240 unidades/s que desaparece al tocar terreno sólido.
- `hitsPlayer()` informa del impacto y la Scene decide. Provisional hasta tener vidas: el impacto devuelve al jugador al inicio, igual que R.
- Dos shooters con posiciones fijas en `Scene::init`: pasillo inferior (col 14) y repisa superior (col 24). No están en `level01.txt` porque `build_world1.py` lo sobrescribe.
- Sprites de Sneed (personaje custom de terceros, no de Bomberman) generados con `tools/build_shooter.py` en `shooter-atlas.png` y `shooter-bullet.png`.
- Aviso: el PNG fuente de Sneed en la copia de trabajo (795x1024 RGB, captura con el fondo de cuadros) difiere del versionado en git (94x121 RGBA con transparencia). El script usa la copia de trabajo y elimina el fondo por relleno desde los bordes.
- Las explosiones matan shooters: mientras dura el fuego (0.5 s), el shooter cuyo cuerpo toca una celda de la cruz se elimina con sus balas. No reaparecen con R.
- Pendiente: daño por contacto, vidas, reaparición de enemigos al reiniciar y añadir los shooters al formato de nivel.

## Jugador con sprites de Rick (3 noviembre)

- Bubble sustituido por Rick (hoja custom de Rick & Morty de Mister Mike, de terceros, no de Bomberman). `tools/build_player.py` genera `rick-atlas.png`: quieto, 3 fotogramas de caminar, subida y caída, en ambas direcciones.
- El script lee la hoja original 851x84 del commit 95db676, porque la copia de trabajo es una captura reescalada x1.2 con fondo de cuadros. Celdas de 24x24 dibujadas en el quad de 32x32. Física y hitbox sin cambios.
- Nuevas animaciones de salto (vy < 0) y caída cuando el jugador no está en el suelo.

## HUD, vidas y daño (3 noviembre)

- Nueva clase `Hud` (`Hud.h/.cpp`): franja superior de 48 px con vidas (corazones llenos o vacíos de `Pixel Heart Sprite Sheet 16x16.png`, sin medios), tiempo restante, nivel, enemigos restantes, alcance de la explosión y capacidad de bombas simultáneas.
- Texto con una fuente pixel 5x7 propia, generada por `tools/build_hud_font.py` en `hud-font.png`. Un único sprite con una animación por carácter.
- La cámara se centra en la zona visible bajo el HUD y puede subir 48 px por encima del mapa para ver la primera fila.
- Tres vidas (`Player::MAX_LIVES`). El jugador pierde una por bala, por contacto con un shooter, por el fuego de una explosión (también la suya) o por agotar el tiempo.
- Al perder una vida reaparece en el spawn con 2 s de invulnerabilidad (parpadeo). Enemigos, bombas y tiempo siguen igual.
- Provisional hasta tener menú: sin vidas se recarga el nivel (mapa, shooters, tiempo, vidas y mejoras).
- Tiempo límite en `Scene::LEVEL_TIME_SECONDS` (180 s). Al llegar a 0 se pierde una vida y el reloj vuelve a empezar.
- G alterna la invulnerabilidad (el HUD muestra INMUNE). Bloquea todo el daño, incluido el del tiempo.

## Jerarquía de enemigos y Walker (3 noviembre)

- Nueva clase base `Enemy` (`Enemy.h/.cpp`). Guarda el cuerpo, la dirección, el sprite y el mapa. Ofrece `applyGravity`, `walk`, `floorAhead`, `touches` y `updateSprite`. Métodos virtuales: `update` (puro), `render`, `hitsPlayer` y `clearProjectiles`.
- `Shooter` hereda de `Enemy` sin cambiar su comportamiento.
- Nuevo `Walker` (estilo goomba, 32x32, 50 unidades/s): camina, gira solo al chocar con una pared y cae por los bordes. Sprite provisional `bub.png`, el antiguo Bubble.
- La Scene guarda `std::vector<Enemy*>`. Daño por contacto o bala, muerte por explosión y contador del HUD valen para cualquier tipo.
- Spawns provisionales en `Scene::spawnEnemies`: 2 shooters y 2 walkers (suelo derecho col 42 y plataforma central col 16).
- Pendiente: tercer tipo de enemigo y sprite definitivo del walker.

## Objetos en arbustos (3 noviembre)

- Cuatro objetos (`Item.h`): Fire Up (+1 alcance), Bomb Up (+1 bomba simultánea), corazón (recupera una vida, máximo 3) y estrella (10 s invulnerable sin parpadeo; el HUD muestra INMUNE con la cuenta atrás).
- Están escondidos en arbustos concretos (`Scene::spawnItems`): Fire Up (3,24), Bomb Up (4,22), corazón (11,20), estrella (15,16). Aparecen cuando el fuego destruye su arbusto y se recogen al tocarlos. El fuego no los destruye.
- `tools/build_items.py` genera `items-atlas.png`. Fire Up y Bomb Up salen de las imágenes generadas con IA `*_gemini.jpeg`: se quitan el fondo de cuadros y el halo usando el contorno como barrera, con cierre morfológico. El corazón sale de la hoja de corazones y la estrella de `varied.png`.
- Al reiniciar el nivel (sin vidas) los objetos vuelven a esconderse. Al perder una vida se conservan.

## Verificación realizada

- Objetos: Debug y Release compilan sin avisos y superan `--smoke-test`. Una captura con Fire Up y Bomb Up revelados temporalmente muestra los sprites en sus celdas; después se restauró el código. Sin prueba manual de revelar, recoger ni de los efectos.

- Enemigos: Debug y Release compilan sin avisos y superan `--smoke-test`. Prueba lógica temporal con stubs superada: los casos del Shooter siguen pasando tras el refactor. El Walker recorre el suelo de pared a pared (cols 1-46), cae de la plataforma central y `touches()` detecta el contacto.

- HUD/vidas: Debug y Release compilan y superan `--smoke-test`. La captura muestra el HUD completo sobre el mapa. Sin prueba manual todavía de daño, reaparición, parpadeo, G, fin de tiempo ni reinicio del nivel.

- Rick y muerte por explosión: Debug y Release compilan y superan `--smoke-test`. La captura muestra a Rick con transparencia. La muerte por explosión todavía no está probada ni en un test automático ni a mano.

- Shooter: prueba lógica temporal (Shooter.cpp con stubs de Sprite/TileMap sobre la colisión real de level01) superada. Cubre patrulla sin caer, giro en pared, repisa, impacto tras la reacción, jugador detrás, a otra altura, fuera de alcance, muro intermedio y bala destruida por pared.
- Shooter: Debug y Release compilan sin avisos nuevos. Ambos superan `--smoke-test` y la captura muestra el sprite con transparencia sobre el suelo.

- Ajustado el ciclo de animación: tres fotogramas a 1 fps, una sola actualización por ciclo, y ampliación de la bomba en los últimos 0.5 s de mecha; al detonar se detiene el fuego.
- La prueba de bomba cubre mecha, fase de explosión, alcance configurable y aterrizaje/salto sobre la plataforma.
- Los recortes de bomba/explosión usan transparencia para el fondo de color clave.
- Debug y Release compilan correctamente con MSBuild de VS 18 tras corregir el recorte de fuego y la restauración del fondo.
- Ambas versiones arrancan en modo oculto `--smoke-test`, cargan recursos, renderizan y terminan con código 0.
- Captura real de Debug anterior a esta iteración: mapa y personaje provisional visibles.
- Tests C++ de suelo, salto, paredes, techo, caída rápida, plataformas, salida de borde y timing fijo: PASS.
- La prueba de salida de plataforma necesitó 1.6 s para dar tiempo a aterrizar; 1 s era insuficiente.
- Pendiente prueba manual del usuario: controles, alcance de explosión, bloqueo por terreno, destrucción de follaje y aspecto de sprites.
- SOIL.lib antiguo produce avisos de PDB ausentes y Edit-and-Continue en Release. No impiden compilar ni ejecutar.
- El sandbox bloqueó el servicio de símbolos (LNK1101 MSPDB140.DLL); la compilación fuera de él pasó.

## Integración de tiles (25 octubre)

- 16 recortes de 16x16 conservan exactamente los píxeles de la fuente (comparación automatizada).
- Ruta de salto desde spawn a la plataforma superior y a ambos lados validada con Movement.h.
- Pruebas del cargador: entrada truncada, ID inválido y spawn bloqueado rechazados.
- El fondo y las cascadas son atravesables; columnas y plataformas son sólidas.
- No se alteraron las físicas ni los controles del jugador.
- Debug y Release compilan correctamente, con los avisos conocidos de SOIL.
- Ambas versiones arrancan con el nuevo mapa y terminan el smoke test con código 0.
- Captura real del mapa en Debug revisada: tiles, capas, cámara y personaje visibles.

## Rediseño del nivel 1 (2 noviembre)

- Sustituida la escalera simétrica por tres ruinas asimétricas con cámaras y arcos huecos.
- Saltos alternados dentro del arco oeste; pozo central visitable; ascenso hasta la puerta este.
- Ruta alternativa por las copas, repisa lateral y paso bajo continuo para recuperarse al caer.
- Cascadas vinculadas a cornisas y vegetación apoyada en plataformas.
- Misma cuadrícula 48x26, mismos tiles, físicas y controles. No se añadió lógica de enemigos ni bombas.
- `tools/build_world1.py --layout-only` regenera el nivel y la vista general sin extraer el atlas otra vez.
- Prueba con la física real: salida y cinco hitos alcanzables; 1436 posiciones de apoyo muestreadas
  alcanzadas y ninguna sin ruta de vuelta al inicio en el grafo de pruebas.
- Actualizado el test para que incluya caminar fuera de una plataforma y caer como acción válida.
- Compilaciones Debug y Release y pruebas de arranque oculto correctas en la nueva raíz Bomberman. Solo avisos conocidos de símbolos SOIL.

## Pulido del mapa y edición gráfica (3 noviembre)

- Cámara 10x9 bloques de mundo, HUD fuera del área de juego; ventana 640x624.
- Arbustos sólidos, destruibles con la explosión, liberan colisión y mantienen sus power-ups.
- El alcance incluye el primer arbusto o cabecera de cascada aunque sean sólidos, y se detiene allí.
- Cascadas detectadas por secuencias 7/8.../9; bloquean hasta que su pie se retrae durante 1000 ms.
- Tras abrir, cabecera y pie quedan adyacentes y físicos. Reiniciar el nivel restaura las cascadas.
- Apoyos junto a las tres cabeceras; primera cascada al lado del comienzo.
- Tiled: src/levels/level01.tmj, conversor tools/tiled_map.py, guía docs/EDITAR_MAPAS.md.
- ITEMS opcional en el formato: mantiene los cuatro objetos y permite moverlos con puntos en Tiled.
- Prueba de terreno tras abrir obstáculos: rutas y regreso al inicio; 1432 posiciones muestreadas.
- Pruebas CPU de colisión, destrucción, rango del fuego, apertura de cascadas, reinicio y cámara.
- Pruebas originales de movimiento y bombas superadas.
- Tiled: ida y vuelta sin cambios, movimiento conjunto de arbusto/objeto y rechazo de datos inválidos.
- Debug y Release compilan y arrancan. Debug se comprobó como BombermanMapTest.exe porque la partida
  abierta bloqueaba Bomberman.exe; hay que cerrar esa partida antes de compilar el nombre habitual.
- Capturas reales de --map-effects-test revisadas en 0, 496 y 1000 ms: retracción visible y dos bloques al final.
- SOIL Release precompilada era incompatible (C1047). Se compilan sus cuatro fuentes C incluidos con
  el mismo compilador del proyecto; no se modifica la biblioteca fuente. Persisten avisos heredados
  de tipos numéricos y del runtime estático de GLEW.
- ActualizarMapa.cmd permite convertir el .tmj guardado con doble clic.

## Próximo paso

Probar manualmente las cascadas, los arbustos y el zoom. Las bombas, los dos tipos de enemigos,
el HUD, las vidas y la recogida de objetos del compañero siguen integrados.
Continuar con los requisitos pendientes (tercer enemigo, pantallas, cinco niveles, jefe y polish).

## Trabajo entre chats

Un chat por fase o bloque importante. Leer AGENTS.md, este archivo y REQUISITOS.md
al comenzar. No asumir que un chat nuevo recuerda toda la conversación anterior.
Evitar que dos chats editen simultáneamente los mismos archivos de esta carpeta.

## Corrección del ejecutable de Visual Studio (3 noviembre)
- El usuario estaba lanzando el antiguo build/Debug/Bomberman.exe (19:29), mientras que los cambios se habían probado como BombermanMapTest.exe.
- Se reconstruyó y enlazó el nombre habitual Bomberman.exe. Sus pruebas --smoke-test y --map-effects-test terminaron con código 0.
- Iniciar sin depurar ya puede lanzar la versión actualizada sin elegir otra copia.
