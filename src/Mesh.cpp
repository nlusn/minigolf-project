#include "Mesh.h"
#include <glm/glm.hpp>

void Mesh::create(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    int stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    indexCount = (int)indices.size();
}

void Mesh::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

static void addVertex(std::vector<float>& v, glm::vec3 p, glm::vec3 n, float u, float t) {
    v.insert(v.end(), {p.x, p.y, p.z, n.x, n.y, n.z, u, t});
}

static void addQuad(std::vector<float>& v, std::vector<unsigned int>& idx,
                    glm::vec3 corner, glm::vec3 sideA, glm::vec3 sideB, glm::vec3 normal,
                    float uMax, float vMax) {
    unsigned int start = (unsigned int)(v.size() / 8);
    addVertex(v, corner, normal, 0, 0);
    addVertex(v, corner + sideA, normal, uMax, 0);
    addVertex(v, corner + sideA + sideB, normal, uMax, vMax);
    addVertex(v, corner + sideB, normal, 0, vMax);
    idx.insert(idx.end(), {start, start + 1, start + 2, start, start + 2, start + 3});
}

Mesh makeBox(float w, float h, float d, float tile) {
    std::vector<float> v;
    std::vector<unsigned int> idx;
    float x = w / 2, y = h / 2, z = d / 2;
    addQuad(v, idx, {-x, -y, z}, {w, 0, 0}, {0, h, 0}, {0, 0, 1}, w / tile, h / tile);    // front
    addQuad(v, idx, {x, -y, -z}, {-w, 0, 0}, {0, h, 0}, {0, 0, -1}, w / tile, h / tile);  // back
    addQuad(v, idx, {x, -y, z}, {0, 0, -d}, {0, h, 0}, {1, 0, 0}, d / tile, h / tile);    // right
    addQuad(v, idx, {-x, -y, -z}, {0, 0, d}, {0, h, 0}, {-1, 0, 0}, d / tile, h / tile);  // left
    addQuad(v, idx, {-x, y, z}, {w, 0, 0}, {0, 0, -d}, {0, 1, 0}, w / tile, d / tile);    // top
    addQuad(v, idx, {-x, -y, -z}, {w, 0, 0}, {0, 0, d}, {0, -1, 0}, w / tile, d / tile);  // bottom
    Mesh m;
    m.create(v, idx);
    return m;
}
