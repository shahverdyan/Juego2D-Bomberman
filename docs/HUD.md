# HUD de Pocket Bomberman y corazón dorado (4 noviembre 2026)

Barra inferior rosa, de una sola fila. Usa iconos 8x8 de la tercera fila de la hoja
`Game Boy _ GBC - Pocket Bomberman - Characters - Overworld (Color).gif`.
El atlas de HUD conserva los píxeles de los iconos; solo se transparenta su fondo rosa
para integrarlos con el fondo de la captura. Números 8x8 basados en el estilo de la referencia.
Catálogo: hud-original-atlas.json.

La fila lógica mide 160x8; se dibuja a 640x32, con escala entera x4.
Orden: reloj + m:ss, corazón si está activo a la izquierda de cara + vidas de reserva, T + enemigos,
bomba + capacidad simultánea y símbolo de explosión + alcance.
El área de juego conserva 10x9 bloques (640x576) sobre la barra; ventana total 640x608.

## Vidas y protección

- Internamente quedan 3 intentos al iniciar; el número visible muestra 2, luego 1 y 0.
- Con el número 0 sigue quedando la última vida. Morir en ella agota los tres intentos.
- El corazón dorado no recupera ni añade vidas. Concede una protección de un solo golpe.
- Si ya tienes corazón, otro no acumula protecciones.
- Al recibir daño con corazón, desaparece el icono, se conservan las vidas y la posición,
  y el personaje parpadea durante 2000 ms. Durante ese tiempo no vuelve a recibir daño.
- Sin corazón, el golpe consume una vida y se mantiene la reaparición del prototipo.
- El modo G y la estrella conservan su protección: un daño bloqueado no consume el corazón.
- El reinicio completo restaura 3 vidas internas y elimina el corazón.

El objeto que sale de los arbustos usa el quinto recuadro dorado de la hoja original, conservando
las posiciones y los demás objetos de tu compañero. `ITEM_HEART` y el tipo 2 de Tiled
mantienen su identificador; cambia su efecto a protección, no a recuperar una vida.

## Comprobación

`tests/player_health_test.cpp`: cuenta 2-1-0, consumo del corazón, ausencia de vidas extra,
no apilamiento, parpadeo/inmunidad durante 2000 ms y reinicio.
`--hud-test`: prueba oculta de integración con recogida real del objeto, daño sin mover
al jugador, capturas con corazón, sin corazón, parpadeo y contadores 1 y 0.

Resultado: pruebas CPU e integración en Debug y Release superadas. Capturas revisadas; la barra real con corazón coincide exactamente con hud-preview.png.

Actualización: el reloj empieza en 4:00; el corazón ocupa la celda x=56, a la izquierda de la cara x=64.
