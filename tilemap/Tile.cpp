// Tile.cpp
#include "Tile.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include <iostream>

Tile::Tile(const std::string& caminhoTileset, GLuint shaderProgram) : shader(shaderProgram) {
    int w, h, canais;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* img = stbi_load(caminhoTileset.c_str(), &w, &h, &canais, STBI_rgb_alpha);
    if (!img) {
        std::cerr << "Erro carregando tileset." << std::endl;
        return;
    }

    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(img);

    float vertices[] = {
        // x, y,      u, v
        -64,  32,    0.0f, 0.0f,
         64,  32,    1.0f / 7, 0.0f,
         64, -32,    1.0f / 7, 1.0f,

         64, -32,    1.0f / 7, 1.0f,
        -64, -32,    0.0f, 1.0f,
        -64,  32,    0.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Tile::desenhar(int indice, glm::vec2 pos) {
    float offset = 1.0f / 7.0f * indice;
//
//    float texMat[8] = {
//        offset,     0.0f,
//        offset + 1.0f / 7.0f, 0.0f,
//        offset + 1.0f / 7.0f, 1.0f,
//        offset + 1.0f / 7.0f, 1.0f,
//        offset,     1.0f,
//        offset,     0.0f
//    };

    glBindTexture(GL_TEXTURE_2D, textura);
    glBindVertexArray(VAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // Sobrescreve temporariamente buffer de coordenadas de textura (opcionalmente mais performático com shader)
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

