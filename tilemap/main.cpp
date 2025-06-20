// main.cpp (com sprite do jogador sobre o tile)
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include "Tile.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int LARGURA = 800;
const int ALTURA = 600;
const int TILE_W = 128;
const int TILE_H = 64;

int mapa[3][3] = {
    {1, 1, 4},
    {4, 1, 4},
    {4, 4, 1}
};

int jogadorI = 1;
int jogadorJ = 1;
float tempoAnterior = 0.0f;

bool podeMover(float tempoAtual) {
    if (tempoAtual - tempoAnterior > 0.15f) {
        tempoAnterior = tempoAtual;
        return true;
    }
    return false;
}

void processarInput(GLFWwindow* janela) {
    float t = glfwGetTime();
    if (!podeMover(t)) return;

    if (glfwGetKey(janela, GLFW_KEY_W) == GLFW_PRESS) jogadorI--;
    if (glfwGetKey(janela, GLFW_KEY_S) == GLFW_PRESS) jogadorI++;
    if (glfwGetKey(janela, GLFW_KEY_A) == GLFW_PRESS) jogadorJ--;
    if (glfwGetKey(janela, GLFW_KEY_D) == GLFW_PRESS) jogadorJ++;

    if (glfwGetKey(janela, GLFW_KEY_Q) == GLFW_PRESS) { jogadorI--; jogadorJ--; }
    if (glfwGetKey(janela, GLFW_KEY_E) == GLFW_PRESS) { jogadorI--; jogadorJ++; }
    if (glfwGetKey(janela, GLFW_KEY_Z) == GLFW_PRESS) { jogadorI++; jogadorJ--; }
    if (glfwGetKey(janela, GLFW_KEY_C) == GLFW_PRESS) { jogadorI++; jogadorJ++; }

    jogadorI = std::max(0, std::min(2, jogadorI));
    jogadorJ = std::max(0, std::min(2, jogadorJ));
}

GLuint criarShader(const char* vs, const char* fs);

const char* vsSrc = R"(
#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tex;

uniform mat4 model;
uniform mat4 projection;

out vec2 TexCoord;
void main() {
    gl_Position = projection * model * vec4(pos, 0.0, 1.0);
    TexCoord = tex;
})";

const char* fsSrc = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D tileset;
void main() {
    FragColor = texture(tileset, TexCoord);
})";

int main() {
    glfwInit();
    GLFWwindow* janela = glfwCreateWindow(LARGURA, ALTURA, "Tilemap Isometrico", NULL, NULL);
    glfwMakeContextCurrent(janela);
    glewInit();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint shader = criarShader(vsSrc, fsSrc);
    glm::mat4 proj = glm::ortho(0.0f, float(LARGURA), float(ALTURA), 0.0f, -1.0f, 1.0f);
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

    Tile tile("imagens/tilesetIso.png", shader);

    // Sprite do jogador
    GLuint texturaSprite;
    glGenTextures(1, &texturaSprite);
    glBindTexture(GL_TEXTURE_2D, texturaSprite);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int sw, sh, sc;
    unsigned char* spriteData = stbi_load("imagens/sprite.png", &sw, &sh, &sc, STBI_rgb_alpha);
    if (!spriteData) {
        std::cerr << "Erro ao carregar sprite.png" << std::endl;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sw, sh, 0, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
    stbi_image_free(spriteData);

    float spriteVertices[] = {
        -64,  64,   0.0f, 0.0f,
         64,  64,   1.0f, 0.0f,
         64, -64,   1.0f, 1.0f,

         64, -64,   1.0f, 1.0f,
        -64, -64,   0.0f, 1.0f,
        -64,  64,   0.0f, 0.0f
    };

    GLuint spriteVAO, spriteVBO;
    glGenVertexArrays(1, &spriteVAO);
    glGenBuffers(1, &spriteVBO);
    glBindVertexArray(spriteVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spriteVertices), spriteVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(janela)) {
        processarInput(janela);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                int tipo = (i == jogadorI && j == jogadorJ) ? 6 : mapa[i][j];
                float x = (j - i) * (TILE_W / 2.0f) + LARGURA / 2 - TILE_W / 2;
                float y = (j + i) * (TILE_H / 2.0f) + 100;
                tile.desenhar(tipo, glm::vec2(x, y));
            }
        }

// Desenhar sprite do jogador
float x = (jogadorJ - jogadorI) * (TILE_W / 2.0f) + LARGURA / 2 - TILE_W / 2;
float y = (jogadorJ + jogadorI) * (TILE_H / 2.0f) + (TILE_H / 2); // Ajuste para base do tile
// Ajustes finos para centralização
x += TILE_W / 2 - 64;  // Centraliza na largura (sprite tem 128 de largura)
y -= 0;              // Ajuste para altura da sprite
glBindTexture(GL_TEXTURE_2D, texturaSprite);
glBindVertexArray(spriteVAO);
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
glDrawArrays(GL_TRIANGLES, 0, 6);


        glfwSwapBuffers(janela);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

GLuint criarShader(const char* vs, const char* fs) {
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vs, NULL);
    glCompileShader(v);

    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fs, NULL);
    glCompileShader(f);

    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}
