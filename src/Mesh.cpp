#include "Mesh.h"
#include <glm/glm.hpp>
#include <cmath>

const float PI = 3.14159265f;

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

Mesh makePlane(float w, float d, float tile) {
    std::vector<float> v;
    std::vector<unsigned int> idx;
    addQuad(v, idx, {-w / 2, 0, d / 2}, {w, 0, 0}, {0, 0, -d}, {0, 1, 0}, w / tile, d / tile);
    Mesh m;
    m.create(v, idx);
    return m;
}

Mesh makeSphere(float radius, int slices, int stacks) {
    std::vector<float> v;
    std::vector<unsigned int> idx;
    for (int i = 0; i <= stacks; i++) {
        float phi = PI * i / stacks;
        for (int j = 0; j <= slices; j++) {
            float theta = 2 * PI * j / slices;
            glm::vec3 n(sin(phi) * cos(theta), cos(phi), -sin(phi) * sin(theta));
            addVertex(v, n * radius, n, (float)j / slices, 1.0f - (float)i / stacks);
        }
    }
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            unsigned int a = i * (slices + 1) + j;
            unsigned int b = a + slices + 1;
            idx.insert(idx.end(), {a, b, a + 1, a + 1, b, b + 1});
        }
    }
    Mesh m;
    m.create(v, idx);
    return m;
}

Mesh makeCylinder(float r1, float r2, int slices) {
    std::vector<float> v;
    std::vector<unsigned int> idx;
    for (int j = 0; j <= slices; j++) {
        float theta = 2 * PI * j / slices;
        float c = cos(theta), s = -sin(theta);
        glm::vec3 n = glm::normalize(glm::vec3(c, r1 - r2, s));  // tilted when it's a cone
        addVertex(v, {r1 * c, 0, r1 * s}, n, 3.0f * j / slices, 0);
        addVertex(v, {r2 * c, 1, r2 * s}, n, 3.0f * j / slices, 1);
    }
    for (int j = 0; j < slices; j++) {
        unsigned int a = j * 2;
        idx.insert(idx.end(), {a, a + 2, a + 1, a + 1, a + 2, a + 3});
    }
    if (r2 > 0) {  // top cap
        unsigned int center = (unsigned int)(v.size() / 8);
        addVertex(v, {0, 1, 0}, {0, 1, 0}, 0.5f, 0.5f);
        for (int j = 0; j <= slices; j++) {
            float theta = 2 * PI * j / slices;
            addVertex(v, {r2 * cos(theta), 1, -r2 * sin(theta)}, {0, 1, 0}, 0.5f + 0.5f * cos(theta), 0.5f + 0.5f * sin(theta));
        }
        for (int j = 0; j < slices; j++)
            idx.insert(idx.end(), {center, center + 1 + j, center + 2 + j});
    }
    Mesh m;
    m.create(v, idx);
    return m;
}

Mesh makeDisc(float radius, int slices) {
    std::vector<float> v;
    std::vector<unsigned int> idx;
    addVertex(v, {0, 0, 0}, {0, 1, 0}, 0.5f, 0.5f);
    for (int j = 0; j <= slices; j++) {
        float theta = 2 * PI * j / slices;
        addVertex(v, {radius * cos(theta), 0, -radius * sin(theta)}, {0, 1, 0}, 0.5f + 0.5f * cos(theta), 0.5f + 0.5f * sin(theta));
    }
    for (int j = 0; j < slices; j++)
        idx.insert(idx.end(), {0u, (unsigned int)(1 + j), (unsigned int)(2 + j)});
    Mesh m;
    m.create(v, idx);
    return m;
}
