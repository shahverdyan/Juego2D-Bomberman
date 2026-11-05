#pragma once

// Música y efectos de sonido con miniaudio (libs/miniaudio, un solo archivo).
// Es un singleton para que cualquier clase pueda pedir un sonido:
//     Audio::instance().play(Audio::JUMP);
// Los archivos están en sounds/ (autores en sounds/Creditos_Audio.txt).
//
// Solo suena una pista de música a la vez. Hay dos tipos:
//   - música de fondo, que se repite en bucle (menú, nivel, jefe);
//   - jingles, que suenan una vez (nivel superado, game over, victoria).
// Si el audio no se puede iniciar (o en las pruebas automáticas), todo es mudo.
class Audio {
public:
    enum Music {
        NO_MUSIC, MENU_MUSIC, LEVEL_MUSIC, BOSS_MUSIC,
        LEVEL_CLEAR_JINGLE, GAME_OVER_JINGLE, VICTORY_JINGLE, MUSIC_COUNT
    };
    enum Sound {
        BOMB_PLACE, EXPLOSION, JUMP, POWER_UP, SHOOTER_SHOT, PLAYER_HIT, ENEMY_DEATH,
        DOOR_OPEN, STATUE_LAND, BOSS_HURT, BOSS_TELEPORT, BOSS_SHOOT, MENU_MOVE, MENU_SELECT,
        SOUND_COUNT
    };

    static Audio& instance();

    // enabled = false: no se abre el dispositivo de audio y nada suena.
    void init(bool enabled);
    void shutdown();

    // Empieza una pista (loop: en bucle). Si ya suena la misma, no hace nada.
    void playMusic(Music music, bool loop);
    void stopMusic();
    // Para la pista actual si es un jingle (no la música en bucle).
    void stopJingle();
    bool isJinglePlaying() const;

    // Efecto de sonido. Si ya estaba sonando, vuelve a empezar.
    void play(Sound sound);

private:
    Audio() = default;
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;
    // Pista que suena ahora mismo (NO_MUSIC si ninguna, o si el jingle ya acabó).
    Music currentMusic() const;

    // Los tipos de miniaudio solo se ven en Audio.cpp (Impl) para no incluir su
    // cabecera enorme en todo el proyecto.
    struct Impl;
    Impl* impl = nullptr;
};
