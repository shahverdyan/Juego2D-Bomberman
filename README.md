# Bomberman - FIB

Abre `Bomberman.sln` en Visual Studio, selecciona Debug o Release y Win32.
Compila y ejecuta con Ctrl+F5. Ambas configuraciones usan MSVC v145 y Windows SDK 10.0.

Los recursos se copian automáticamente a `build/Debug` o `build/Release`.
El ejecutable localiza los recursos junto a sí mismo, también al abrirlo desde el explorador.
La aplicación actualmente es un prototipo de movimiento; aún no hay bombas ni enemigos.

- Flechas izquierda/derecha o A/D: moverse.
- Espacio, flecha arriba o W: saltar al pulsar (mantener no repite saltos).
- R: regresar al inicio.
- Escape: salir.

## Carpetas

- `src`: código derivado de 02-Bubble y recursos.
- `src/images/tilesets`: imagen fuente, atlas extraído y catálogo de tiles originales.
- `src/levels/level01.txt`: mapa propio del mundo 1, en formato TILEMAP2.
- `libs`: bibliotecas del laboratorio, sin duplicarlas.
- `docs`: PDF, requisitos, estado y guía de mapas.
- `tests`: pruebas de movimiento sin OpenGL.
- `build`: resultados generados, excluidos de Git.

Consulta `docs/MAPAS.md` antes de editar el mapa. El terreno usa tiles originales de Pocket Bomberman; el personaje sigue siendo Bubble.

## Verificación

Desde Developer Command Prompt for VS, situado en esta carpeta:

```
MSBuild Bomberman.sln /p:Configuration=Debug /p:Platform=Win32
MSBuild Bomberman.sln /p:Configuration=Release /p:Platform=Win32
cl /nologo /EHsc /std:c++14 /Fe:build\movement_test.exe /Fo:build\movement_test.obj tests\movement_test.cpp
build\movement_test.exe
build\Debug\Bomberman.exe --smoke-test
```

La última orden abre un contexto OpenGL oculto, renderiza cinco frames, guarda
`smoke.log` y `smoke.ppm` junto al ejecutable y termina. No sustituye una prueba manual.
La biblioteca SOIL suministrada no incluye sus antiguos PDB: el linker puede avisar
de símbolos ausentes aunque el juego compile y funcione.

El ejecutable de esta fase no es todavía el paquete final de entrega.
