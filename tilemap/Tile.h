// Tile.h
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Tile {
public:
    Tile(const std::string& caminhoTileset, GLuint shader);
    void desenhar(int indice, glm::vec2 pos);

private:
    GLuint textura;
    GLuint VAO;
    GLuint shader;
};
