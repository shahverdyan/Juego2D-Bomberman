# Sin bomba al reaparecer (6 noviembre 2026)

- El Espacio pulsado durante la animación de muerte quedaba guardado en Game::pressed y Player::update lo usaba en
  el primer frame tras reaparecer, poniendo una bomba en el inicio. Player::updateDeath ahora lo descarta.
- Verificado: Debug y Release compilan sin avisos; cinco pruebas ocultas con código 0. Pendiente probarlo jugando.

# Sin jingle al entrar en la sala del jefe (6 noviembre 2026)

- Game::update: al superar un nivel cuyo siguiente es el último (Scene::LEVEL_COUNT, el jefe) no suena
  LEVEL_CLEAR_JINGLE; el fundido entra directo y updateMusic pone BOSS_MUSIC. El resto de niveles mantiene el jingle.
- Verificado: Debug y Release compilan sin avisos; cinco pruebas ocultas con código 0. Pendiente escucharlo jugando.

# Sala del jefe ampliada (6 noviembre 2026)

- level06 pasa a 15x13 con seis pares de plataformas. Era inválido porque ITEMS apuntaba a (2,6) y (12,6), y el
  cargador exige que cada objeto esté sobre un arbusto; los arbustos están en (1,6) y (13,6). Objetos movidos ahí.
- Verificado: el cargador real (LevelData) acepta level01, level02 y level06; la versión anterior da inválido.
  Debug y Release compilados y copiados a build; cinco pruebas ocultas con código 0 en ambos.
- level06.tmj regenerado desde el .txt (to-tiled); ida y vuelta por Tiled identica y tests/tiled_map_test.py PASS.
  Ojo: tools/build_boss_arena.py aun escribe la sala vieja de 10x9 si se ejecuta.
- Rediseno en Tiled (arbustos en FOREGROUND): los puntos Fire Up y Bomb Up estaban una casilla a la derecha de sus
  arbustos; movidos a (1,5) y (7,5) y .txt regenerado. LevelData lo acepta; Debug y Release compilan y pasan las pruebas.
- Recordatorio: editar un .txt de nivel no basta; hay que compilar (o Recompilar en Visual Studio) para copiarlo a build.

# Avisos de variables sin inicializar (6 noviembre 2026)

- `Game::bPlay` se inicializa a `true` en su declaración (Game.h) y `Scene::currentTime` a 0 en el
  constructor de Scene. Release compila y pasa las cinco pruebas ocultas; el enlace de Debug falló porque
  `build/Debug/Bomberman.exe` estaba abierto (LNK1168): pendiente recompilar Debug con el juego cerrado.

# Enemigos y objetos de Forest (6 noviembre 2026)

- Koopas añadidos en (20, 43) y (19, 24). Tirador en (7, 45) en la casilla del antiguo arbusto.
- Objetos nuevos en arbustos. Bomb Up en (10, 38), corazón dorado en (3, 30) y Fire Up en (9, 29). Coordenadas en orden fila, columna.
- Forest suma nueve enemigos y siete objetos ocultos. Verificados conversión de Tiled, recorridos del nivel y arranque en Debug y Release.

# Niveles desde archivo, teclas 1-5 y jefe en el nivel 6 (6 noviembre 2026)

- Teclas 1-5: cargan ese nivel desde cualquier pantalla (aunque esté bloqueado, y lo desbloquean). Desde un menú
  empiezan partida nueva; jugando conservan vidas y mejoras. Si el nivel aún no existe, no hacen nada.
- Sin posiciones fijas en el código: se han borrado los enemigos de Forest de `Scene::spawnEnemies` y los objetos
  de `Scene::spawnItems`. `level01.txt` tiene ahora su sección `ENEMIES` (las mismas 6 posiciones).
- El jefe es un enemigo más del archivo: tipo 3 en `ENEMIES` (`LevelData::EnemyKind::Boss`, también en
  `tools/tiled_map.py`). `Scene::spawnBoss(x, y)` lo crea en esa casilla y marca `isBossLevel()`, que usan la
  música del jefe y la victoria automática. Desaparece `Scene::BOSS_LEVEL`.
- La sala del jefe pasa de `level05` a `level06` (`git mv`; `tools/build_boss_arena.py` también escribe level06
  y genera la misma salida). `Scene::LEVEL_COUNT` = 6; la selección sigue con 5 botones, así que al 6 solo se
  llega superando el anterior. Mientras falten los niveles 3-5, `Game::nextLevel()` los salta: Ocean → jefe.
- Ojo: la compilación copia recursos pero no borra; se han quitado a mano los `level05.*` viejos de `build/`.
- Verificado: Debug y Release compilan sin avisos del juego; cinco pruebas ocultas con código 0; tests CPU y
  `tests/tiled_map_test.py` PASS; ida y vuelta por Tiled sin cambios en level01 y level06. Capturas con teclas
  simuladas (código temporal retirado): 1 → Forest con 6 enemigos, 2 → Ocean con 5, y (6 habilitada solo en la
  prueba) → jefe en su casilla atacando.

# Revisión tras la integración (5 noviembre 2026)

- Rama main (merge c16585a): Debug y Release compilan sin avisos en el código del juego; las cinco pruebas
  ocultas dan código 0 en ambos; arranque real sin avisos de audio.
- Pendiente obligatorio: niveles 3 (Wind) y 4 (Cloud); teclas 1-5 para cargar niveles (hecho después, arriba).
- Pendiente de entrega: info.txt, binario autónomo, proyecto compilable, demo.avi y NomCognoms.zip.

# Integración de aleks y cristian

- Se combinan los enemigos y objetos de los mapas de aleks con la carga especial del jefe en el nivel 5. Al reaparecer se restauran el reloj de 4:00 y el estado del encuentro.
- La campaña actual recorre Forest, Ocean y Evil. La selección desbloquea cada nivel al superar el anterior y se saltan los archivos de nivel pendientes.
- Verificados Debug y Release, las cinco pruebas nativas y la conversión de Tiled. La prueba integrada cubre patrullas y reaparición en Ocean, las estatuas y cinco impactos al jefe, victoria y regreso al menú desde Game Over.
- Comprobada la carga de las seis pistas y los efectos de sonido. Las pruebas de audio se hicieron con miniaudio sin dispositivo de salida.

# Música y efectos de sonido (5 noviembre 2026)

- Librería: miniaudio 0.11.21 + stb_vorbis (dominio público, un solo archivo cada una) en `libs/miniaudio`.
  `AudioLibrary.cpp` compila su implementación (avisos silenciados); `Audio.h/.cpp` es la clase del juego
  (singleton): `playMusic(pista, bucle)`, `stopMusic()`, `stopJingle()` y `play(efecto)`.
- Archivos en `src/sounds/music` (.ogg, en streaming) y `src/sounds/sfx` (precargados). El vcxproj copia
  `sounds/` junto al ejecutable. Autores en `src/sounds/Creditos_Audio.txt` y en la pantalla de créditos.
- Música: menú (menú, instrucciones, créditos y selección), nivel (1-4), jefe (5). Jingles de una vez: nivel
  superado (al empezar el siguiente; la música del nivel espera a que acabe), game over y victoria. Todo cambio
  de pantalla corta el jingle anterior (`Game::changeState`); la música de fondo la elige `Game::updateMusic()`.
- Efectos: poner bomba, explosión, salto, recoger objeto, disparo del Shooter, jugador herido, muerte de
  enemigo (por fuego o K), puerta abriéndose, estatua aterrizando, jefe herido / teletransporte / disparo,
  menú mover / elegir (los niveles bloqueados no suenan al elegirlos).
- Las pruebas automáticas (`--smoke-test`...) arrancan el audio desactivado. Si no hay dispositivo de audio
  o falta un archivo, se avisa por consola y el juego sigue mudo en esa parte.
- Créditos: `tools/build_menu.py` añade música y sonidos (líneas a 11 px para que quepan; la fuente no tiene
  punto, por eso "LUKE RUSTLTD").
- Verificado: Debug y Release compilan (solo los avisos previos de SOIL/enlazador), cinco pruebas ocultas con
  código 0, ejecución real sin avisos de carga (y con un archivo quitado a propósito aparece "Audio: falta ...").
- Créditos de audio completos (menu-select: Fupi, menu-hover: leohpaz, boss-shoots: Vircon32).
- Probado jugando por el usuario: volúmenes y sonidos dados por buenos. Más adelante, quizá música por mundo.

# Créditos (5 noviembre 2026)

- `tools/build_menu.py`: créditos con "SPRITES: THE SPRITERS RESOURCE" y "SPRITES CUSTOM: HECHOS CON GEMINI".
  Regenerado `menu-credits.png` (comprobado a la vista: el texto cabe en el recuadro). Las demás imágenes del
  script salen idénticas.

# Progreso de niveles (5 noviembre 2026)

- Solo se puede elegir un nivel en la selección tras superar el anterior (`Game::unlockedLevel`, empieza en 1).
  Al completar un nivel se desbloquea el siguiente y se pasa a él. Los bloqueados se ven oscurecidos.
- Mientras falten niveles, `Game::nextLevel()` salta los que no tienen archivo. Con el mapa de Ocean integrado,
  la secuencia es Forest → Ocean → Evil → victoria.
- El progreso dura mientras el juego está abierto (no se guarda en disco).
- Verificado: Debug y Release compilan, cinco pruebas ocultas con código 0, y captura temporal (retirada) en la
  que Espacio sobre Evil, bloqueado, no inicia el nivel.
- Pendiente: teclas 1-5 para saltar de nivel (exigidas por el enunciado), que deberán ignorar el bloqueo.

# Jefe final (5 noviembre 2026)

- `Boss` (`Boss.h/.cpp`): el demonio del mundo Evil de Pocket Bomberman, en el nivel 5 (`Scene::BOSS_LEVEL`).
  Comportamiento según la descripción del jefe original, con 5 impactos en vez de 9:
  - Flota despacio (45 u/s) por toda la sala hacia puntos al azar, también bajos: ahí se le alcanza con bombas.
  - Tras flotar 2.5 s ataca; una vez sí y otra no, antes se teletransporta: parpadea, desaparece y reaparece
    en otro punto al azar a más de 120 unidades del jugador. Oculto no hace daño ni recibe golpes.
  - Ataques por turnos, con las alas levantadas: dos bolas grandes que rebotan por la sala 7 s; cuatro bolas
    en diagonal (en X) que estallan contra el terreno; y una estatua que cae del techo encima del jugador
    (antes eran dos; reducido a una).
- 5 vidas (`MAX_HEALTH`). Cada explosión le quita una y queda invulnerable 1.5 s (parpadeo y cara de dolor).
  Al morir parpadea y se hunde; sus estatuas se rompen y, 3 s después (`Scene::BOSS_VICTORY_DELAY_MS`), se pasa
  sola a la pantalla de victoria (en la sala del jefe la puerta se abre pero no hace falta entrar). K lo mata.
- `Statue` (`Statue.h/.cpp`, dibujada con el sprite de la gárgola): parpadea 0.7 s pegada al techo para avisar,
  cae con gravedad, se queda 4 s en el suelo y se rompe. Solo hace daño mientras cae; en el suelo es inofensiva. Las bombas la rompen antes.
- `Enemy` gana ganchos: `hitByExplosion()` (por defecto muere; el jefe pierde una vida),
  `takeSpawnedEnemies()` (el jefe entrega las estatuas; la Scene las añade tras el bucle) y `touches()` virtual.
- `tools/build_boss.py` genera `boss-atlas.png` (4 fotogramas de 32x32: reposo x2, alas levantadas, herido),
  `boss-fireball.png` (6 de 16x16: bola y estallido) y `gargoyle-atlas.png` (3 de 16x16), de la última fila de la
  hoja original Overworld. Se dibujan a escala 2 (jefe 64x64; bolas 32x32 y las grandes 56x56).
- Arena provisional `levels/level05.txt` (+ `.tmj` para Tiled) generada por `tools/build_boss_arena.py`: sala de
  10x9 tiles (una pantalla), dos plataformas, arbustos con Fire Up y Bomb Up y la puerta en el centro.

## Verificación del jefe
- Debug y Release compilan sin avisos. Las cinco pruebas ocultas: código 0 en ambos.
- Capturas con teclas simuladas en modo G (código temporal, retirado): flota por la sala, lanza las dos bolas
  grandes que rebotan en paredes, techo y plataformas, se teletransporta, caen las estatuas (una sobre el
  jugador, otra sobre una plataforma) y dispara las cuatro diagonales.
- Versión anterior (golpes forzados): muere al quinto, HUD a 0, portal abierto.
- Pendiente: prueba manual de dificultad y de la muerte con el nuevo comportamiento (constantes en Boss.h/.cpp
  y Statue.h).

# Terrazas superiores de Ocean (5 noviembre 2026)

- Koopa en fila 8, columna 1. Retirado el arbusto de fila 8, columna 3.
- Tirador en fila 4, columna 14. El arbusto con Fire Up pasa de fila 4, columna 13 a fila 5, columna 8 y ocupa el extremo de la terraza.
- Verificados los puntos de aparición, la bomba debajo del arbusto y el acceso al portal tras romperlo. Conversión de Tiled y recursos de Debug y Release comprobados.

# Koopas de Ocean (5 noviembre 2026)

- Reubicado el Koopa en fila 4, columna 14. Añadido un Koopa en fila 16, columna 10. Ocean tiene tres Koopas y dos tiradores.
- La patrulla al detectar bordes es compartida por ambos tipos en todos los niveles.
- Verificados la conversión de Tiled, la colisión en los puntos de aparición y la copia a Debug y Release.

# Reloj y patrullas al reaparecer (5 noviembre 2026)

- Cada vida empieza con 4:00. Al perder una vida y reaparecer, se restaura el reloj junto con el escenario.
- Koopas y tiradores comprueban el apoyo antes de avanzar y giran ante paredes y bordes. Un enemigo colocado en el aire baja en vertical hasta su primera plataforma.
- Verificados Debug y Release. La prueba de integración recorre las patrullas de Forest y Ocean y comprueba el reloj de 4:00 tras perder una vida.

# Ocean poblado y reaparición (5 noviembre 2026)

- Ocean tiene seis objetos ocultos, dos Koopas y dos tiradores. Las posiciones se editan en OBJECTS y se guardan en ITEMS y ENEMIES.
- El Koopa de fila 22, columna 14 queda protegido por la cascada hasta abrirla. El tirador de fila 28, columna 14 cabe bajo el extremo elevado del muelle. El arbusto de ese extremo pasa a fila 25, columna 13.
- Bombas con 2 s de mecha y 0,4 s de fuego. Máximo de 4 bombas simultáneas y 4 casillas de alcance por dirección.
- Al perder una vida y reaparecer se restauran enemigos, contador, arbustos, objetos, cascadas y portal. Se conservan las mejoras y el tiempo restante.
- Verificados Tiled, rutas de Ocean, tiempos de bombas y físicas. Debug y Release superan las cinco pruebas nativas.
- Prueba de integración con selección de Ocean, apertura del recinto, recogida y topes de mejoras, reaparición completa y fin de la tercera vida.

# Ocean, nivel 2 (5 noviembre 2026)

- Mapa vertical de 16×30 con el atlas World 1, diez arbustos, una cascada retráctil y portal en (10, 4).
- level02.tmj editable y level02.txt convertido. Ocean aparece disponible desde Jugar.
- Inicio provisional en (2, 28), con cero objetos y cero enemigos. Los enemigos fijos pertenecen a Forest.
- Vista con coordenadas en level02-preview.png. Pendiente fijar inicio, objetos y enemigos.
- Verificado con las físicas reales el acceso al portal, a la cabecera de la cascada y a los diez arbustos.
- Debug y Release compilados. Arranque correcto y selección de Ocean comprobada desde el menú.

# Limpieza (5 noviembre 2026)

- `Screen` reduce con GL_LINEAR (MinFilter) y amplía con GL_NEAREST: los JPEG de 1061x1008 (menú, game over,
  victoria) ya no pierden filas y columnas al dibujarse a 640x608; las pantallas pixeladas siguen nítidas.
- `tools/build_menu.py` ya no genera menu-background, game-over ni victory (son JPEG hechos a mano). Solo genera
  cursor, instrucciones, créditos, selección de nivel y botones; regenerados idénticos byte a byte.
- Comentarios de Menu y Screen actualizados (las posiciones del cursor dependen de menu-background.jpeg).
- Verificación: Release compila y pasa las cinco pruebas ocultas; captura del menú con texto y borde uniformes.
  Debug recompilado después con el juego cerrado: las cinco pruebas ocultas con código 0.

# Animaciones de muerte y transiciones (5 noviembre 2026)

- Enemigos: `Enemy::kill()` empieza la muerte (fuego o tecla K). Durante `DEATH_MS` (1 s) no hacen daño ni
  cuentan en el HUD ni para abrir la puerta; después la Scene los borra. Cada tipo tiene su animación:
  Koopa se mete en el caparazón y el caparazón boca abajo salta y cae; Sneed (Shooter) se gira boca abajo,
  salta y cae; Boo cierra los ojos y sube despacio parpadeando cada vez más rápido.
- Jugador: al perder una vida (o acabarse el tiempo) el mundo se congela, Rick pone cara de susto 0.5 s,
  salta y cae fuera de la pantalla (1.8 s en total, como en Mario). Después reaparece con el parpadeo de 2 s,
  o, sin vidas o sin tiempo, aparece GAME OVER. La tecla R no interrumpe la animación.
- Transiciones: todo cambio de pantalla, de nivel y el paso a game over o victoria hace un fundido a negro
  de 300 ms, aplica el cambio con la pantalla negra y vuelve a aparecer en 300 ms. Usa un cuadro negro con
  transparencia (textura de 1x1 y el uniform `color`). Las pruebas ocultas siguen cambiando de estado sin fundido.
- Atlas ampliados (las partes anteriores no cambian, comprobado píxel a píxel): `rick-atlas.png` columna 6
  (susto, x=776 de la hoja de Rick); `koopa-atlas.png` fila 2 (agachado, caparazón con ojos, caparazón boca
  abajo); `boo-atlas.png` columna 5 (ojos cerrados); `shooter-atlas.png` fila 2 (alerta boca abajo).

## Verificación
- Debug y Release compilan sin avisos nuevos. Las cinco pruebas ocultas: código 0 en ambos. Tests CPU PASS.
- Capturas con teclas simuladas (código temporal y spawn movido en la copia de build, todo restaurado):
  fundidos menú → selección → partida; K junto a Sneed y junto al Koopa (HUD a 0 al momento, animación completa);
  tiempo agotado con límite de 2 s: susto, salto, caída, fundido y GAME OVER.
- Pendiente: ver la muerte del Boo en una prueba manual (usa el mismo mecanismo).

# Fin del tiempo = game over (5 noviembre 2026)

- Al llegar el reloj a 0 la partida se pierde directamente (pantalla GAME OVER), sin importar las vidas que
  queden, el corazón dorado o la estrella. Antes se quitaba una vida como un golpe normal, y la estrella, el
  parpadeo o el corazón dorado podían evitarlo. Solo la tecla G (modo de prueba) lo evita: el reloj vuelve a 4:00.
- Verificación: Release compila; `--smoke-test`, `--bomb-collision-test` y `--hud-test` con código 0. Tests CPU PASS.
  Prueba integrada temporal (límite de 2 s, partida con estrella y corazón dorado, teclas simuladas): el HUD baja
  de 0:02 a 0:01 con 2 vidas de reserva y después aparece GAME OVER. Código temporal retirado.
- Debug no se pudo reenlazar porque el juego estaba abierto (LNK1168): cerrar el juego y recompilar Debug.

# Selección de nivel, game over y victoria (5 noviembre 2026)

- Flujo: menú principal → Jugar → selección de nivel → partida. ESC en la partida vuelve a la selección de
  nivel; ESC en la selección vuelve al menú principal.
- `LevelSelect` usa la pantalla original de selección de mundo (torre con Forest, Ocean, Wind, Cloud y Evil =
  niveles 1 a 5). ↑/↓ suben y bajan por la torre; el botón elegido se dibuja con la versión roja del original.
  Los niveles sin archivo `levels/levelNN.txt` se ven oscurecidos (uniform `color`) y no se pueden elegir.
- `Scene` carga niveles por número (`levelFile`, `levelExists`, `startNewGame`, `startLevel`) y expone un
  `Result` (RUNNING, LEVEL_COMPLETED, GAME_OVER) que `Game` consulta tras cada update.
- Al perder las 3 vidas: pantalla GAME OVER (antes se reiniciaba el nivel). Al cruzar el portal se carga el
  siguiente nivel conservando vidas y mejoras; si no existe, pantalla de victoria. Desde ambas, Espacio/Enter/ESC
  vuelven a la selección de nivel.
- `tools/build_menu.py` genera además `level-select.png`, `level-buttons.png`, `game-over.png` y `victory.png`
  (160x152, escala 4). Game over: rótulo y Rick aturdido de la hoja de Rick sobre fondo negro. Victoria: rótulo
  WORLD CLEAR!! y tres Ricks sobre el paisaje de la torre de la intro original. Las piezas de Rick se reducen
  a píxeles nativos, sin el verde de la hoja ni su halo.
- Instrucciones: flechas dibujadas en lugar de la palabra FLECHAS, W más clara (no parecía una U) y fondo del
  recuadro más oscuro para que el texto contraste más.

## Verificación
- Debug y Release compilan sin avisos nuevos. Las cinco pruebas ocultas: código 0 en ambos.
- Flujo con teclas simuladas y capturas (código temporal en main.cpp/Scene.cpp y spawn movido en la copia de
  build, todo restaurado): selección, nivel bloqueado no elegible, ESC entre pantallas, K + portal → victoria
  (no hay nivel 2), game over forzado → selección de nivel.
- Pendiente: enemigos y objetos por nivel (los enemigos siguen fijos para el nivel 1), niveles 2-5, teclas 1-5,
  transiciones y prueba manual completa.

# Menú principal y estados del juego (5 noviembre 2026)

- `Game` tiene una máquina de estados: MENU, INSTRUCTIONS, CREDITS y PLAYING. Cada estado decide qué se
  actualiza y qué se dibuja. Al cambiar de estado se descartan las teclas pendientes (el Espacio que elige
  "Jugar" no pone una bomba).
- El juego empieza en el menú. ↑/↓ o W/S mueven la bomba-cursor; Espacio o Enter eligen.
  "Jugar" llama a `Scene::startNewGame()` (nivel recargado, jugador con valores iniciales).
- ESC: en la partida o en Instrucciones/Créditos vuelve al menú; en el menú cierra el juego.
- Clases nuevas: `Screen` (imagen a pantalla completa) y `Menu` (fondo + cursor animado). Usan el shader de la Scene.
- `tools/build_menu.py` genera `menu-background.png`, `menu-cursor.png`, `menu-instructions.png` y
  `menu-credits.png` (320x304, a escala 2). Fondo: escena del árbol con Rick de `GEMINI_Fotos_Menus_RICK.jpeg`
  (generada con IA) reducida a 160x152. Logo y bomba-cursor: hoja original de menús de Pocket Bomberman.
  Recuadros con los colores del original y texto con la fuente 5x7 del proyecto. Los textos de instrucciones y
  créditos están en listas al principio del script.
- Las pruebas ocultas (`--bomb-collision-test`, `--hud-test`...) empiezan directamente en PLAYING;
  `--smoke-test` ahora captura el menú.

## Verificación del menú
- Debug y Release compilan sin avisos nuevos. Las cinco pruebas ocultas: código 0 en ambos.
- Navegación con teclas simuladas (código temporal en main.cpp, ya retirado) y captura de cada paso:
  ↓ mueve el cursor, Espacio abre Instrucciones, ESC vuelve con el cursor en su sitio, Espacio en "Jugar"
  entra en la partida sin poner bomba, ESC vuelve al menú, ↓↓ + Enter abre Créditos.
- Pendiente: revisar el texto de créditos (nombres de los autores), pantallas de game over y victoria,
  y volver al menú al perder las 3 vidas (ahora se reinicia el nivel). La intro no se ha hecho.

# Puerta-portal y tecla K (5 noviembre 2026)

- Nueva clase `Door` (`Door.h/.cpp`): cerrada, abriéndose (4 fotogramas, 666 ms) y abierta (portal girando).
- La puerta se coloca sobre el marcador del mapa (tile 16, `door_marker`, casilla inferior izquierda de un hueco
  de 2x2). `LevelData::findDoor()` guarda su posición y quita el tile para que no se dibuje. En level01: (43,7).
- Se abre cuando no quedan enemigos. Abierta, si el centro del jugador entra en ella se completa el nivel.
- Provisional hasta tener más niveles: completar el nivel lo recarga conservando vidas y mejoras.
  `restartLevel()` (sin vidas) y `completeLevel()` comparten el nuevo `loadLevel()`.
- Tecla K (`Scene::killAllEnemies`): elimina todos los enemigos y sus balas; la puerta se abre sola.
- `tools/build_portal.py` genera `portal-atlas.png` (7 celdas de 28x32, dibujadas a 56x64) desde
  `Portal_Rick_Gemini.jpeg` (imagen generada con IA): quita el fondo y normaliza el tamaño de los fotogramas.

## Verificación de la puerta
- Debug y Release compilan sin avisos nuevos. `--smoke-test`, `--bomb-collision-test`, `--hud-test` y
  `--map-effects-test`: código 0 en ambos.
- Tests CPU `level_test`, `map_obstacles_test`, `movement_test` y `bomb_test`: PASS.
- Capturas reales con código temporal (jugador junto a la puerta): cerrada sobre la plataforma con enemigos vivos;
  tras K, HUD con 0 enemigos y portal abierto. El código temporal se retiró.
- Pendiente: prueba manual de la animación completa y de entrar en el portal; cambio al siguiente nivel cuando existan.

# Sprite del Walker: Koopa Troopa (5 noviembre 2026)

- El Walker usa ahora `koopa-atlas.png` en lugar del provisional `bub.png`. Lógica y hitbox (32x32) sin cambios.
- `tools/build_koopa.py` recorta los 8 fotogramas de caminar del Koopa verde (segunda fila, grupo derecho) de la hoja
  custom de NO Body (de terceros, no de Bomberman). Escala 1:1, centrados y apoyados abajo en celdas de 32x32;
  fila 0 mira a la izquierda (espejo) y fila 1 a la derecha. El fondo azul uniforme pasa a transparente.
- Animación de 8 pasos a 10 fps.
- Verificación: Debug y Release compilan; `--smoke-test` y `--bomb-collision-test` con código 0 en ambos.
  Captura real con un Koopa colocado temporalmente junto al spawn: apoyado en el suelo, orientado y con transparencia.

# Tercer enemigo: Boo (5 noviembre 2026)

- Nueva clase `Boo` (`Boo.h/.cpp`) que hereda de `Enemy`. Comportamiento del Boo de Mario:
  vuela sin gravedad y atraviesa el terreno; persigue al jugador cuando este le da la espalda
  (aceleración 150 u/s², máximo 70 u/s) y frena hasta pararse tapándose la cara cuando el jugador lo mira.
- Siempre se gira hacia el jugador. Solo despierta si el jugador está a 10 tiles (320 u) o menos.
- Hitbox de 28x24 y dibujo de 40x32 centrado. Daño por contacto y muerte por explosión con la lógica común de la Scene.
- `Enemy::update` recibe ahora también `playerFacingLeft` (nuevo `Player::isFacingLeft()`); Shooter y Walker lo ignoran.
- Spawns provisionales en `Scene::spawnEnemies`: (20,15) y (38,5), en zonas abiertas.
- `tools/build_boo.py` genera `boo-atlas.png` (5x2 celdas de 20x16): 4 fotogramas persiguiendo y 1 escondido,
  y la fila espejo. Sprite de la hoja custom de Teeks (de terceros, no de Bomberman); el fondo blanco se quita
  rellenando desde el borde para conservar el blanco del cuerpo.

## Verificación del Boo
- Debug y Release compilan sin avisos nuevos. `--smoke-test` y `--bomb-collision-test`: código 0 en ambos.
- Prueba CPU temporal (Boo.cpp/Enemy.cpp reales con stubs de Sprite/TileMap): persigue en x e y, respeta la
  velocidad máxima, se para y se esconde al ser mirado, no despierta lejos, ignora el terreno y persigue en ambos sentidos.
- Captura real con un Boo colocado temporalmente junto al spawn: transparencia correcta, mira al jugador y se esconde.
- Debug se compiló con la versión final antes de la captura; el último reenlace de Debug falló porque el juego estaba abierto
  (LNK1168). Hay que cerrar el juego y recompilar Debug.
- Pendiente: prueba manual de persecución, daño y muerte por explosión; spawns definitivos por nivel.

## Registro anterior

# Alineación de bloques y edición de píxeles (4 noviembre 2026)

- Corregido origen del recorte del escenario de (15,13) a (15,12): dieciséis bloques
  completos de 16x16, en el mismo atlas de 64x64 y con los mismos IDs.
- Arbusto, cielo, tronco superior, plataformas, puerta, suelo base y pie de cascada
  coinciden con las siete referencias aportadas. El pie tiene 16 filas opacas.
- level01.tmj y level01.txt conservan sus bytes: el cambio de imagen se comparte
  automáticamente con este mapa y cualquier otro que use el tileset World 1.
- build_world1.py --atlas-only permite restaurar los recortes sin regenerar el mapa.
  build_original_effects.py ya no sobrescribe el terreno al generar bombas y objetos.
- Guía EDITAR_MAPAS.md ampliada con tamaño nativo, cuadrícula, archivo de píxeles
  compartido y aplicación al juego.

## Registro anterior

# Recursos originales y animación (4 noviembre 2026)

- Corregido pie de cascada: 15 filas de imagen más una transparente; sin césped en el borde.
- Tres bombas originales, con centro/base fijos y pulso que acelera antes de detonar.
- Explosión original ensamblada en siete tipos de piezas y tres estados, con crecimiento y retirada.
- Animación basada en el tiempo de cada bomba; una bomba nueva no reinicia otras explosiones.
- Fire Up, Bomb Up, corazón y objeto de invulnerabilidad usan los recuadros dorados solicitados.
- Generadores actualizados: build_original_effects.py, build_items.py, build_world1.py y build_original_hud.py.
- Se mantienen el mapa, las colisiones, los alcances y los efectos de los objetos.
- Pruebas de assets y temporización de bombas superadas. Ver EFECTOS_ORIGINALES.md.

## Registro anterior

# Ajustes actuales: tiempo, HUD y bombas (4 noviembre 2026)

- Cada nivel empieza con 4:00 (240 s), también al reiniciarlo.
- El corazón dorado se dibuja a la izquierda de la cara de vidas; se conserva la cuenta 2/1/0.
- La bomba usa la casilla que contiene el centro de la hitbox, no su esquina ni el borde adelantado.
- Las bombas activas son sólidas para el jugador por arriba, ambos lados y debajo.
- Al colocar una bomba dentro del personaje, se permite salir de ella una vez; después impide volver
  a entrar. Saltar para salir permite aterrizar encima. La misma excepción evita atascarse si se reaparece dentro de una bomba.
- Una bomba que ya está explotando deja de actuar como bloque; su fuego conserva el daño existente.
- Pruebas CPU de colocación con pequeños desplazamientos a ambos lados, caras de colisión, salida del propietario y explosión.
- Modo oculto --bomb-collision-test comprueba la Scene y el Player reales, no solo las funciones auxiliares.

## Registro anterior

# HUD y protección: estado actual (4 noviembre 2026)

- HUD inferior rosa de una fila, con iconos originales extraídos de la hoja Overworld (Color).
- Reloj + tiempo, cara + vidas 2/1/0, corazón dorado cuando está activo, T + enemigos,
  bomba + capacidad y explosión + alcance. Números estilo 8x8 de la referencia.
- Ventana 640x608: 10x9 bloques de juego (640x576) y barra inferior de 32 px.
- El corazón dorado absorbe un impacto sin consumir vidas ni cambiar la posición.
- Tras perderlo, parpadeo e inmunidad durante 2 s. No acumula corazones ni suma vidas.
- El resto de power-ups y la invulnerabilidad por estrella/G se conservan.
- Modelo CPU PlayerHealth.h y pruebas de salud; modo oculto --hud-test para QA integrada.
- Ver docs/HUD.md para assets, reglas y verificación.
- Verificados los ejecutables habituales build/Debug/Bomberman.exe y build/Release/Bomberman.exe: compilación, arranque y --hud-test correctos.
- La prueba integrada recoge el corazón, recibe daño sin perder vidas ni cambiar posición, captura el parpadeo y comprueba los contadores 1 y 0.
- La captura real con corazón coincide píxel a píxel con docs/hud-preview.png; los iconos conservan sus píxeles originales y los otros tres power-ups no cambiaron.

## Registro anterior

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
- Ve al jugador solo si está delante, a VISION_RANGE como máximo (media pantalla: 5 tiles con 10 columnas; antes 8), en la línea de la bala y sin tiles sólidos en medio. Entonces se para.
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
- Tercer tipo (Boo) y sprite del walker (Koopa) añadidos el 5 de noviembre.

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

## Verificación de estos ajustes
- Bombas y movimiento: pruebas CPU superadas. Debug y Release compilados con el nombre habitual Bomberman.exe.
- Arranque, prueba integrada de colocación/colisiones y HUD: código 0 en ambas configuraciones.

## Verificación de recursos originales
- Ejecutables habituales Debug y Release compilados y arrancados. Pruebas de colisiones, cascadas y secuencia gráfica original: código 0 en ambos.

## Verificación de alineación de tiles
- Las siete capturas coinciden píxel a píxel a resolución nativa. Los 16 tiles conservan 16x16 píxeles opacos de la fuente.
- Regenerar bombas/objetos no sobrescribe el terreno. Pruebas de assets y conversión de Tiled superadas.
- level01.tmj, level01.txt y los atlas de bombas, explosiones y objetos conservan sus hashes.
- Debug y Release: compilación/copia de recursos, arranque y apertura de cascadas con código 0. Ambas salidas contienen el atlas actualizado.

