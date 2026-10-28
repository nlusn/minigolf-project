#pragma once
#include <glad/gl.h>
#include <vector>

// vertex = position[3] + normal[3] uv[2]
class Mesh {
public:
    unsigned int vao = 0, vbo = 0, ebo = 0;
    int indexCount = 0;

    void create(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void draw() const;
};

Mesh makeBox(float width, float height, float depth, float tile = 1.0f);
