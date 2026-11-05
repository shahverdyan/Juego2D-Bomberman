// Compila una sola vez el código de las librerías de audio de libs/miniaudio:
// miniaudio (reproducción y .wav) y stb_vorbis (lectura de .ogg). Son de dominio
// público. No es código del juego: Audio.cpp es quien las usa.
// Se silencian sus avisos para no mezclarlos con los del proyecto.
#pragma warning(push, 0)

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#pragma warning(pop)
