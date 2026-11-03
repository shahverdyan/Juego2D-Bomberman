# Bomberman - FIB

Abre `Bomberman.sln` en Visual Studio, selecciona Debug o Release y Win32.
Compila y ejecuta con Ctrl+F5. Ambas configuraciones usan MSVC v145 y Windows SDK 10.0.

Los recursos se copian automáticamente a `build/Debug` o `build/Release`.
El ejecutable localiza los recursos junto a sí mismo, también al abrirlo desde el explorador.
La aplicación es un prototipo de movimiento y lógica inicial de bombas; todavía no hay enemigos
ni daño al jugador. La bomba y su explosión se muestran con fotogramas del atlas suministrado.

- Flechas izquierda/derecha o A/D: moverse.
- Espacio: colocar una bomba al pulsar (mecha de 3 s; mantener no repite).
- Flecha arriba o W: saltar al pulsar; se puede aterrizar sobre bombas.
- La explosión inicial tiene alcance 1 (una celda por dirección), se detiene ante terreno sólido y elimina follaje decorativo.
- La capacidad y el alcance tienen valores y métodos de mejora para futuros power-ups.
- R: regresar al inicio.
- Escape: salir.

## Carpetas

- `src`: código derivado de 02-Bubble y recursos.
- `src/images/tilesets`: imagen fuente, atlas extraído y catálogo de tiles originales.
- `src/levels/level01.txt`: mapa propio del mundo 1, en formato TILEMAP2.
- `libs`: bibliotecas del laboratorio, sin duplicarlas.
- `docs`: PDF, requisitos, estado y guía de mapas.
- `tests`: pruebas de movimiento y lógica de bombas sin OpenGL.
- `build`: resultados generados, excluidos de Git.

Consulta `docs/MAPAS.md` antes de editar el mapa. El terreno usa tiles originales de Pocket Bomberman; el personaje sigue siendo Bubble.

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
La biblioteca SOIL suministrada no incluye sus antiguos PDB: el linker puede avisar
de símbolos ausentes aunque el juego compile y funcione.

El ejecutable de esta fase no es todavía el paquete final de entrega.
