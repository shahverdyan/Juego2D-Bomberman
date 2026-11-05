#pragma once
#include <string>
#include "Sprite.h"

// Pantalla fija: una imagen estirada sobre toda la ventana (menú, instrucciones,
// créditos, game over...). Puede ser pequeña y pixelada (se amplía sin suavizar)
// o grande, como los JPEG hechos a mano (se reduce suavizando).
class Screen {
public:
    Screen() = default;
    ~Screen();
    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;

    void init(const std::string& imageFile, ShaderProgram& program);
    void render();

    // Prepara el shader para dibujar en coordenadas de ventana (0..640, 0..608).
    static void beginFullWindow(ShaderProgram& program);

private:
    Texture texture;
    Sprite* sprite = nullptr;
    ShaderProgram* program = nullptr;
};
