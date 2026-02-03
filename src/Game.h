#pragma once

#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GLFWwindow;

class Game {
public:
    Game(int width, int height);
    ~Game();

    void processInput(GLFWwindow* window, float deltaTime);
    void update(float deltaTime);
    void render();
    void resize(int width, int height);

    bool shouldClose() const { return m_shouldClose; }
    void setShouldClose(bool value) { m_shouldClose = value; }

    int scoreLeft() const { return m_scoreLeft; }
    int scoreRight() const { return m_scoreRight; }

private:
    int m_width, m_height;
    bool m_shouldClose = false;

    Renderer m_renderer;
    glm::mat4 m_view;
    glm::mat4 m_projection;

    // Ball - manual physics (no Box2D for reliable paddle collision)
    glm::vec2 m_ballPos;
    glm::vec2 m_ballVel;

    // Paddle positions
    float m_paddleLeftY;
    float m_paddleRightY;
    int m_scoreLeft;
    int m_scoreRight;
    float m_tableLength;
    float m_tableWidth;
    float m_paddleHeight;
    float m_paddleDepth;
    float m_ballRadius;
    float m_aiTargetOffset = 0.0f;
    float m_aiMistakeTimer = 0.0f;

    void resetBall();
    void updateAIPaddle(float deltaTime);
};
