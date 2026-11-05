#include <stdexcept>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Screen.h"
#include "ViewConfig.h"

Screen::~Screen() {
    delete sprite;
}

void Screen::init(const std::string& imageFile, ShaderProgram& shaderProgram) {
    program = &shaderProgram;
    if (!texture.loadFromFile(imageFile, TEXTURE_PIXEL_FORMAT_RGBA))
        throw std::runtime_error("Cannot load " + imageFile);
    // Ampliar sin suavizar mantiene los píxeles nítidos (pantallas de 320x304 o
    // 160x152). Al reducir (JPEG de 1061x1008) se suaviza: con GL_NEAREST se
    // saltarían filas y columnas enteras y el texto se vería irregular.
    texture.setMinFilter(GL_LINEAR);
    texture.setMagFilter(GL_NEAREST);
    sprite = Sprite::createSprite(glm::vec2(float(view::WINDOW_WIDTH), float(view::WINDOW_HEIGHT)),
        glm::vec2(1.f, 1.f), &texture, program);
    sprite->setNumberAnimations(1);
    sprite->setAnimationSpeed(0, 1);
    sprite->addKeyframe(0, glm::vec2(0.f, 0.f));
    sprite->changeAnimation(0);
    sprite->setPosition(glm::vec2(0.f, 0.f));
}

void Screen::render() {
    beginFullWindow(*program);
    sprite->render();
}

void Screen::beginFullWindow(ShaderProgram& program) {
    glViewport(0, 0, view::WINDOW_WIDTH, view::WINDOW_HEIGHT);
    program.use();
    program.setUniformMatrix4f("projection",
        glm::ortho(0.f, float(view::WINDOW_WIDTH), float(view::WINDOW_HEIGHT), 0.f));
    program.setUniform4f("color", 1.f, 1.f, 1.f, 1.f);
}
