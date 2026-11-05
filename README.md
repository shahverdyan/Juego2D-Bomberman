# Bomberman - FIB

Abre `Bomberman.sln` en Visual Studio, selecciona Debug o Release y Win32.
Compila y ejecuta con Ctrl+F5. Ambas configuraciones usan MSVC v145 y Windows SDK 10.0.

Los recursos se copian automáticamente a `build/Debug` o `build/Release`.
El ejecutable localiza los recursos junto a sí mismo, también al abrirlo desde el explorador.
La aplicación incluye bombas, enemigos, HUD, vidas, objetos y obstáculos interactivos del mapa.
La cámara muestra 10x9 bloques de juego, con HUD independiente.

- Flechas izquierda/derecha o A/D: moverse.
- Espacio: colocar una bomba al pulsar (mecha de 3 s; mantener no repite).
- Flecha arriba o W: saltar al pulsar; se puede aterrizar sobre bombas.
- La explosión inicial tiene alcance 1 (una celda por dirección), se detiene ante terreno sólido y destruye arbustos físicos; una explosión en la cabecera de una cascada la retrae en 1 s.
- La capacidad y el alcance tienen valores y métodos de mejora para futuros power-ups.
- R: regresar al inicio.
- G: invulnerabilidad. K: eliminar a todos los enemigos (abre el portal).
- Menú y selección de nivel: flechas arriba/abajo o W/S para elegir, Espacio o Enter para aceptar.
- Escape: desde la partida, game over o victoria vuelve a la selección de nivel; desde ella,
  instrucciones o créditos vuelve al menú principal; en el menú principal, salir.

## Carpetas

- `src`: código derivado de 02-Bubble y recursos.
- `src/images/tilesets`: imagen fuente, atlas extraído y catálogo de tiles originales.
- `src/levels/level01.txt`: mapa propio del mundo 1, en formato TILEMAP2.
- `libs`: bibliotecas del laboratorio, sin duplicarlas.
- `docs`: PDF, requisitos, estado y guía de mapas.
- `tests`: pruebas de movimiento y lógica de bombas sin OpenGL.
- `build`: resultados generados, excluidos de Git.

Consulta `docs/MAPAS.md` antes de editar el mapa. El terreno usa tiles originales de Pocket Bomberman.
Para editar con Tiled, abre src/levels/level01.tmj y consulta docs/EDITAR_MAPAS.md.

## Verificación

Desde Developer Command Prompt for VS, situado en esta carpeta:

```
MSBuild Bomberman.sln /p:Configuration=Debug /p:Platform=Win32
MSBuild Bomberman.sln /p:Configuration=Release /p:Platform=Win32
cl /nologo /EHsc /std:c++14 /Fe:build\movement_test.exe /Fo:build\movement_test.obj tests\movement_test.cpp
build\movement_test.exe
cl /nologo /EHsc /std:c++14 /Fe:build\bomb_test.exe /Fo:build\bomb_test.obj tests\bomb_test.cpp
build\bomb_test.exe
build\Debug\Bomberman.exe --smoke-test
```

La última orden abre un contexto OpenGL oculto, renderiza cinco frames, guarda
`smoke.log` y `smoke.ppm` junto al ejecutable y termina. No sustituye una prueba manual.
SOIL se compila desde los fuentes C incluidos para evitar incompatibilidades de compilador.
Las bibliotecas antiguas pueden producir avisos de tipos o símbolos sin impedir la ejecución.

El ejecutable de esta fase no es todavía el paquete final de entrega.

Edición gráfica: abre src/levels/level01.tmj en Tiled, guarda y ejecuta ActualizarMapa.cmd.
Después cierra la partida anterior y compila. Guía completa: docs/EDITAR_MAPAS.md.

HUD inferior original: tiempo, vidas 2/1/0, protección dorada, enemigos, bombas y alcance.
El corazón absorbe un golpe sin sumar vidas y activa 2 s de parpadeo. Ver docs/HUD.md.

Nivel: 4 minutos. Bombas sólidas por las cuatro caras; colocación por el centro del personaje.
Al colocarla se permite salir una vez para no quedar encerrado, y después bloquea el paso.
