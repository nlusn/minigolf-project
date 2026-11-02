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
Mesh makeSphere(float radius, int slices, int stacks);
Mesh makeCylinder(float bottomRadius, float topRadius, int slices);  // height 1, standing on y = 0
Mesh makePlane(float width, float depth, float tile = 1.0f);
Mesh makeDisc(float radius, int slices);
