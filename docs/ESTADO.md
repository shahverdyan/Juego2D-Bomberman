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

## Verificación realizada

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

## Próximo paso

Probar manualmente el alcance de la explosión, el bloqueo por terreno y la destrucción visual del follaje.
Añadir daño, bloques destructibles y recogida de power-ups.
Sustituir el personaje provisional cuando se disponga de su spritesheet. El mapa todavía
no tiene enemigos ni condiciones para abrir la puerta, y no constituye por sí solo un nivel completo del enunciado.

## Trabajo entre chats

Un chat por fase o bloque importante. Leer AGENTS.md, este archivo y REQUISITOS.md
al comenzar. No asumir que un chat nuevo recuerda toda la conversación anterior.
Evitar que dos chats editen simultáneamente los mismos archivos de esta carpeta.
