@echo off
setlocal
where py.exe >nul 2>nul
if errorlevel 1 goto bundled
py -3 "%~dp0tools\tiled_map.py" to-game "%~dp0src\levels\level01.tmj" "%~dp0src\levels\level01.txt"
goto result
:bundled
if not exist "%USERPROFILE%\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe" goto systempython
"%USERPROFILE%\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe" "%~dp0tools\tiled_map.py" to-game "%~dp0src\levels\level01.tmj" "%~dp0src\levels\level01.txt"
goto result
:systempython
python "%~dp0tools\tiled_map.py" to-game "%~dp0src\levels\level01.tmj" "%~dp0src\levels\level01.txt"
:result
if errorlevel 1 goto failed
echo Mapa actualizado. Cierra la partida abierta y compila en Visual Studio para probarlo.
goto finish
:failed
echo No se ha actualizado el mapa. Revisa el error mostrado arriba y la guia docs\EDITAR_MAPAS.md.
:finish
pause
