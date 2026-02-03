#include "Game.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

Game::Game(int width, int height)
    : m_width(width), m_height(height),
      m_ballPos(0, 0), m_ballVel(0, 0),
      m_paddleLeftY(0), m_paddleRightY(0),
      m_scoreLeft(0), m_scoreRight(0),
      m_tableLength(20.0f), m_tableWidth(12.0f),
      m_paddleHeight(2.5f), m_paddleDepth(0.5f),
      m_ballRadius(0.4f) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    m_view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 25.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    float halfH = (float)height / 2.0f;
    float halfW = (float)width / 2.0f;
    float aspect = halfW / halfH;
    float viewHeight = 14.0f;
    m_projection = glm::ortho(-aspect * viewHeight, aspect * viewHeight, -viewHeight, viewHeight, 0.1f, 100.0f);

    m_renderer.setView(m_view);
    m_renderer.setProjection(m_projection);

    resetBall();
}

Game::~Game() = default;

void Game::resetBall() {
    m_ballPos = glm::vec2(0.0f, 0.0f);
    m_ballVel.x = (std::rand() % 2 == 0 ? 1.0f : -1.0f) * 10.0f;
    m_ballVel.y = ((std::rand() % 100) / 50.0f - 1.0f) * 5.0f;
}

void Game::processInput(GLFWwindow* window, float deltaTime) {
    const float paddleSpeed = 14.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_paddleLeftY += paddleSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_paddleLeftY -= paddleSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        m_shouldClose = true;

    float limit = (m_tableWidth - m_paddleHeight) / 2.0f;
    m_paddleLeftY = std::clamp(m_paddleLeftY, -limit, limit);
    m_paddleRightY = std::clamp(m_paddleRightY, -limit, limit);
}

void Game::updateAIPaddle(float deltaTime) {
    const float aiSpeed = 11.0f;
    const float mistakeRange = 2.5f;
    const float mistakeChangeInterval = 0.35f;
    float limit = (m_tableWidth - m_paddleHeight) / 2.0f;

    if (m_ballVel.x > 0) {
        m_aiMistakeTimer += deltaTime;
        if (m_aiMistakeTimer >= mistakeChangeInterval) {
            m_aiMistakeTimer = 0.0f;
            m_aiTargetOffset = (std::rand() / (float)RAND_MAX - 0.5f) * 2.0f * mistakeRange;
        }

        float targetY = m_ballPos.y + m_aiTargetOffset;
        targetY = std::clamp(targetY, -limit - 1.0f, limit + 1.0f);

        float diff = targetY - m_paddleRightY;
        if (std::abs(diff) > 0.15f) {
            float move = std::copysign(std::min(aiSpeed * deltaTime, std::abs(diff)), diff);
            m_paddleRightY = std::clamp(m_paddleRightY + move, -limit, limit);
        }
    }
}

void Game::update(float deltaTime) {
    deltaTime = std::min(deltaTime, 0.05f);

    float halfLen = m_tableLength / 2.0f;
    float halfWidth = m_tableWidth / 2.0f;
    float paddleHalfH = m_paddleHeight / 2.0f;

    // Paddle collision bounds: paddle mesh X [-10,-9.5] left, [9.5,10] right. Extend toward center to prevent tunneling.
    const float pad = m_ballRadius * 1.2f;
    const float leftPaddleMinX = -halfLen;
    const float leftPaddleMaxX = -halfLen + m_paddleDepth + pad;
    const float rightPaddleMinX = halfLen - m_paddleDepth - pad;
    const float rightPaddleMaxX = halfLen;

    const float fixedDt = 1.0f / 600.0f;
    float accumulated = deltaTime;
    int steps = 0;
    const int maxSteps = 160;

    while (accumulated >= fixedDt && steps < maxSteps) {
        steps++;
        accumulated -= fixedDt;

        glm::vec2 prevPos = m_ballPos;
        m_ballPos += m_ballVel * fixedDt;

        // Walls
        if (m_ballPos.y + m_ballRadius > halfWidth) {
            m_ballPos.y = halfWidth - m_ballRadius;
            m_ballVel.y = -std::abs(m_ballVel.y);
        }
        if (m_ballPos.y - m_ballRadius < -halfWidth) {
            m_ballPos.y = -halfWidth + m_ballRadius;
            m_ballVel.y = std::abs(m_ballVel.y);
        }

        float leftPaddleMinY = m_paddleLeftY - paddleHalfH;
        float leftPaddleMaxY = m_paddleLeftY + paddleHalfH;
        float rightPaddleMinY = m_paddleRightY - paddleHalfH;
        float rightPaddleMaxY = m_paddleRightY + paddleHalfH;

        // Swept AABB: ball's path from prevPos to m_ballPos
        float bMinX = std::min(prevPos.x, m_ballPos.x) - m_ballRadius;
        float bMaxX = std::max(prevPos.x, m_ballPos.x) + m_ballRadius;
        float bMinY = std::min(prevPos.y, m_ballPos.y) - m_ballRadius;
        float bMaxY = std::max(prevPos.y, m_ballPos.y) + m_ballRadius;

        const float speedBoost = 1.08f;
        const float maxSpeed = 28.0f;

        // Left paddle
        const float leftPaddleFaceX = -halfLen + m_paddleDepth;
        if (m_ballVel.x < 0 &&
            bMinX < leftPaddleMaxX && bMaxX > leftPaddleMinX &&
            bMinY < leftPaddleMaxY && bMaxY > leftPaddleMinY) {
            m_ballPos.x = leftPaddleFaceX + m_ballRadius + 0.01f;
            float newSpeed = std::min(std::sqrt(m_ballVel.x * m_ballVel.x + m_ballVel.y * m_ballVel.y) * speedBoost, maxSpeed);
            float scale = newSpeed / std::sqrt(m_ballVel.x * m_ballVel.x + m_ballVel.y * m_ballVel.y);
            m_ballVel.x = std::abs(m_ballVel.x) * scale;
            m_ballVel.y *= scale;
        }

        // Right paddle
        const float rightPaddleFaceX = halfLen - m_paddleDepth;
        if (m_ballVel.x > 0 &&
            bMinX < rightPaddleMaxX && bMaxX > rightPaddleMinX &&
            bMinY < rightPaddleMaxY && bMaxY > rightPaddleMinY) {
            m_ballPos.x = rightPaddleFaceX - m_ballRadius - 0.01f;
            float newSpeed = std::min(std::sqrt(m_ballVel.x * m_ballVel.x + m_ballVel.y * m_ballVel.y) * speedBoost, maxSpeed);
            float scale = newSpeed / std::sqrt(m_ballVel.x * m_ballVel.x + m_ballVel.y * m_ballVel.y);
            m_ballVel.x = -std::abs(m_ballVel.x) * scale;
            m_ballVel.y *= scale;
        }
    }

    // Score
    if (m_ballPos.x < -halfLen) {
        m_scoreRight++;
        resetBall();
    }
    if (m_ballPos.x > halfLen) {
        m_scoreLeft++;
        resetBall();
    }

    updateAIPaddle(deltaTime);
}

void Game::render() {
    m_renderer.clear();

    glm::mat4 tableModel = glm::scale(
        glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.1f)),
        glm::vec3(m_tableLength, m_tableWidth, 0.2f)
    );
    m_renderer.drawCube(tableModel, glm::vec3(0.2f, 0.6f, 0.2f));

    float borderH = 0.05f;
    glm::mat4 borderModel = glm::scale(
        glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, borderH / 2)),
        glm::vec3(m_tableLength + 0.5f, m_tableWidth + 0.5f, borderH)
    );
    m_renderer.drawCube(borderModel, glm::vec3(0.25f, 0.2f, 0.15f));

    // Paddles and debug: use exact collider bounds (same formula as update())
    float halfLenR = m_tableLength / 2.0f;
    float paddleHalfH = m_paddleHeight / 2.0f;
    float pad = m_ballRadius * 1.2f;

    float leftMinX = -halfLenR;
    float leftMaxX = -halfLenR + m_paddleDepth + pad;
    float leftMinY = m_paddleLeftY - paddleHalfH;
    float leftMaxY = m_paddleLeftY + paddleHalfH;

    float rightMinX = halfLenR - m_paddleDepth - pad;
    float rightMaxX = halfLenR;
    float rightMinY = m_paddleRightY - paddleHalfH;
    float rightMaxY = m_paddleRightY + paddleHalfH;

    m_renderer.drawRectFilled(leftMinX, leftMinY, leftMaxX, leftMaxY, 0.0f, glm::vec3(1.0f, 0.2f, 0.2f));
    m_renderer.drawRectFilled(rightMinX, rightMinY, rightMaxX, rightMaxY, 0.0f, glm::vec3(0.2f, 0.2f, 1.0f));

    glm::vec3 ballPosRaised(m_ballPos.x, m_ballPos.y, 0.15f);
    glm::mat4 ballModel = glm::translate(
        glm::scale(glm::mat4(1.0f), glm::vec3(m_ballRadius * 2.5f)),
        ballPosRaised
    );
    m_renderer.drawCube(ballModel, glm::vec3(1.0f, 1.0f, 0.0f));

    glm::vec3 debugColor(1.0f, 0.0f, 1.0f);
    m_renderer.drawRectFilled(leftMinX, leftMinY, leftMaxX, leftMaxY, 0.2f, debugColor);
    m_renderer.drawRectFilled(rightMinX, rightMinY, rightMaxX, rightMaxY, 0.2f, debugColor);

    m_renderer.drawScore(m_scoreLeft, m_scoreRight, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void Game::resize(int width, int height) {
    m_width = width;
    m_height = height;
    float halfH = (float)height / 2.0f;
    float halfW = (float)width / 2.0f;
    float aspect = halfW / halfH;
    float viewHeight = 14.0f;
    m_projection = glm::ortho(-aspect * viewHeight, aspect * viewHeight, -viewHeight, viewHeight, 0.1f, 100.0f);
    m_renderer.setProjection(m_projection);
}
