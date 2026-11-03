#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <vector>




void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		Game::instance().keyPressed(key);
	else if (action == GLFW_RELEASE)
		Game::instance().keyReleased(key);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Game::instance().mouseMove(int(xpos), int(ypos));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		Game::instance().mousePress(button);
	else if (action == GLFW_RELEASE)
		Game::instance().mouseRelease(button);
}


int main(int argc, char** argv)
{
    // Locate resources beside the executable, also when launched from Explorer.
    char executable[MAX_PATH];
    GetModuleFileNameA(NULL, executable, MAX_PATH);
    std::string directory(executable);
    directory = directory.substr(0, directory.find_last_of("/\\"));
    SetCurrentDirectoryA(directory.c_str());
    const bool smoke = argc > 1 && std::string(argv[1]) == "--smoke-test";
    if (smoke) freopen("smoke.log", "w", stdout);
    GLFWwindow* window;
	double timePreviousFrame, currentTime;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	if (smoke) glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bomberman | Arrows/A-D: move | Space: Place Bomb | R: reset", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Set window initial position */
	glfwSetWindowPos(window, 100, 100);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Set callbacks */
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	/* Init glew to have access to GL extensions */
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { glfwTerminate(); return 1; }
    glfwSwapInterval(1);
    try {

	/* Init step of the game loop */
	Game::instance().init();
    timePreviousFrame = glfwGetTime();
    double accumulator = 0.0;
    const double fixedStep = 0.008; // 125 Hz; exact integer milliseconds for the lab API.
    int frames = 0;
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        accumulator += std::min(0.25, currentTime - timePreviousFrame);
        timePreviousFrame = currentTime;
        glfwPollEvents();
        while (accumulator >= fixedStep) {
            if (!Game::instance().update(8)) glfwSetWindowShouldClose(window, GLFW_TRUE);
            accumulator -= fixedStep;
        }
        Game::instance().render();
        if (smoke && frames == 4) {
            std::vector<unsigned char> pixels(SCREEN_WIDTH * SCREEN_HEIGHT * 3);
            glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
            std::ofstream capture("smoke.ppm", std::ios::binary);
            capture << "P6\n" << SCREEN_WIDTH << " " << SCREEN_HEIGHT << "\n255\n";
            for (int y = SCREEN_HEIGHT - 1; y >= 0; --y)
                capture.write(reinterpret_cast<char*>(pixels.data() + y * SCREEN_WIDTH * 3), SCREEN_WIDTH * 3);
        }
        glfwSwapBuffers(window);
        if (smoke && ++frames >= 5) break;
    }
    Game::instance().shutdown();
    if (smoke) std::cout << "SMOKE OK: resources, shaders, map and rendering loaded" << std::endl;
    } catch (const std::exception& error) {
        std::cerr << error.what() << std::endl;
        if (smoke) std::cout << error.what() << std::endl;
        else MessageBoxA(NULL, error.what(), "Bomberman: loading error", MB_OK | MB_ICONERROR);
        Game::instance().shutdown();
        glfwTerminate();
        return 1;
    }

	glfwTerminate();
	return 0;
}

