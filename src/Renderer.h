#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void drawCube(const glm::mat4& model, const glm::vec3& color);
    void drawQuad(const glm::mat4& model, const glm::vec3& color);
    void drawRectOutline(float minX, float minY, float maxX, float maxY, float z, const glm::vec3& color);
    void drawRectFilled(float minX, float minY, float maxX, float maxY, float z, const glm::vec3& color);
    void drawModelOutline(const glm::mat4& model, const glm::vec3& color);
    void drawDigit(int digit, float x, float y, float size, float z, const glm::vec3& color);
    void drawScore(int left, int right, float z, const glm::vec3& color);
    void setView(const glm::mat4& view);
    void setProjection(const glm::mat4& projection);
    void clear();

private:
    unsigned int cubeVAO, cubeVBO;
    unsigned int quadVAO, quadVBO;
    unsigned int lineVAO, lineVBO;
    unsigned int cubeOutlineVAO, cubeOutlineVBO;
    unsigned int shaderProgram;

    void setupCube();
    void setupQuad();
    void setupLineRect();
    void loadShader();
};
