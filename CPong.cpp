// CPong - Cross-platform 3D Pong
// Windows, macOS, Linux

#include "CPong.h"
#include "src/Game.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (game) game->resize(width, height);
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // OpenGL 3.3 Core for cross-platform compatibility (macOS requires Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    const int width = 1280;
    const int height = 720;
    GLFWwindow* window = glfwCreateWindow(width, height, "3D Pong - CPong", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // VSync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    Game game(width, height);
    glfwSetWindowUserPointer(window, &game);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window) && !game.shouldClose()) {
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        game.processInput(window, deltaTime);
        game.update(deltaTime);
        game.render();

        std::string title = "3D Pong - " + std::to_string(game.scoreLeft()) + " : " + std::to_string(game.scoreRight());
        glfwSetWindowTitle(window, title.c_str());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
