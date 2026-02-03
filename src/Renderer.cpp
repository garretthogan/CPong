#include "Renderer.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

// Cube vertices (position only)
static const float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
};

// Quad vertices (flat rectangle)
static const float quadVertices[] = {
    -0.5f, -0.5f, 0.0f,  0.5f, -0.5f, 0.0f,  0.5f,  0.5f, 0.0f,
     0.5f,  0.5f, 0.0f, -0.5f,  0.5f, 0.0f, -0.5f, -0.5f, 0.0f
};

// Unit rect for line outline (0,0)-(1,1), GL_LINE_LOOP
static const float lineRectVertices[] = {
    0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f
};

// Unit cube top face outline (-0.5 to 0.5), GL_LINE_LOOP - matches paddle/cube local space
static const float cubeTopFaceVertices[] = {
     0.5f,  0.5f, 0.5f,  -0.5f,  0.5f, 0.5f,  -0.5f, -0.5f, 0.5f,   0.5f, -0.5f, 0.5f
};

Renderer::Renderer() : cubeVAO(0), cubeVBO(0), quadVAO(0), quadVBO(0), lineVAO(0), lineVBO(0), cubeOutlineVAO(0), cubeOutlineVBO(0), shaderProgram(0) {
    loadShader();
    setupCube();
    setupQuad();
    setupLineRect();
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
    glDeleteVertexArrays(1, &cubeOutlineVAO);
    glDeleteBuffers(1, &cubeOutlineVBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

void Renderer::loadShader() {
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    shaderProgram = shader.ID;
    shader.ID = 0;  // Prevent Shader destructor from deleting (we manage it)
}

void Renderer::setupCube() {
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Renderer::setupQuad() {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Renderer::setupLineRect() {
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineRectVertices), lineRectVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &cubeOutlineVAO);
    glGenBuffers(1, &cubeOutlineVBO);
    glBindVertexArray(cubeOutlineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeOutlineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTopFaceVertices), cubeTopFaceVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Renderer::drawModelOutline(const glm::mat4& model, const glm::vec3& color) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, &color[0]);
    glBindVertexArray(cubeOutlineVAO);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glLineWidth(1.0f);
}

void Renderer::drawRectOutline(float minX, float minY, float maxX, float maxY, float z, const glm::vec3& color) {
    float w = maxX - minX;
    float h = maxY - minY;
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(minX, minY, z))
        * glm::scale(glm::mat4(1.0f), glm::vec3(w, h, 1.0f));

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, &color[0]);
    glBindVertexArray(lineVAO);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glLineWidth(1.0f);
}

void Renderer::drawRectFilled(float minX, float minY, float maxX, float maxY, float z, const glm::vec3& color) {
    float w = maxX - minX;
    float h = maxY - minY;
    float centerX = (minX + maxX) * 0.5f;
    float centerY = (minY + maxY) * 0.5f;
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(centerX, centerY, z))
        * glm::scale(glm::mat4(1.0f), glm::vec3(w, h, 1.0f));

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, &color[0]);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawCube(const glm::mat4& model, const glm::vec3& color) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, &color[0]);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::drawQuad(const glm::mat4& model, const glm::vec3& color) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, &color[0]);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// 7-segment display: which segments are on for digits 0-9 (a,b,c,d,e,f,g)
static const unsigned char SEGMENTS[] = {
    0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B
};

void Renderer::drawDigit(int digit, float x, float y, float size, float z, const glm::vec3& color) {
    if (digit < 0 || digit > 9) return;
    unsigned char seg = SEGMENTS[digit];
    float t = size * 0.12f;
    float w = size * 0.8f;
    float h = size * 0.4f;

    auto drawSeg = [this, x, y, size, z, &color](float lx, float ly, float lw, float lh) {
        drawRectFilled(x + lx * size, y + ly * size, x + (lx + lw) * size, y + (ly + lh) * size, z, color);
    };

    if (seg & 0x40) drawSeg(0.05f, 1.15f, 0.9f, 0.12f);   // a top
    if (seg & 0x20) drawSeg(0.83f, 0.62f, 0.12f, 0.55f);  // b top-right
    if (seg & 0x10) drawSeg(0.83f, 0.07f, 0.12f, 0.55f);  // c bot-right
    if (seg & 0x08) drawSeg(0.05f, 0.0f, 0.9f, 0.12f);    // d bottom
    if (seg & 0x04) drawSeg(0.0f, 0.07f, 0.12f, 0.55f);   // e bot-left
    if (seg & 0x02) drawSeg(0.0f, 0.62f, 0.12f, 0.55f);   // f top-left
    if (seg & 0x01) drawSeg(0.05f, 0.575f, 0.9f, 0.12f);  // g middle
}

void Renderer::drawScore(int left, int right, float z, const glm::vec3& color) {
    float digitW = 1.2f;
    float gap = 0.5f;
    float totalW = digitW * 4.0f + gap;
    float x = -totalW * 0.5f;
    float y = 5.5f;

    drawDigit(left / 10, x, y, 1.0f, z, color);
    drawDigit(left % 10, x + digitW, y, 1.0f, z, color);
    drawRectFilled(x + digitW * 2 + gap * 0.2f, y + 0.4f, x + digitW * 2 + gap * 0.8f, y + 0.6f, z, color);
    drawDigit(right / 10, x + digitW * 2 + gap, y, 1.0f, z, color);
    drawDigit(right % 10, x + digitW * 3 + gap, y, 1.0f, z, color);
}

void Renderer::setView(const glm::mat4& view) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
}

void Renderer::setProjection(const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
