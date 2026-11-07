#include "Hud.h"
#include "Font.h"

void Hud::init() {
    shader.load("shaders/hud.vert", "shaders/hud.frag");
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Hud::begin(int screenWidth, int screenHeight) {
    width = screenWidth;
    height = screenHeight;
    vertices.clear();
}

void Hud::rect(float x, float y, float w, float h, glm::vec4 c) {
    float corners[6][2] = {{x, y}, {x, y + h}, {x + w, y + h}, {x, y}, {x + w, y + h}, {x + w, y}};
    for (auto& p : corners)
        vertices.insert(vertices.end(), {p[0], p[1], c.r, c.g, c.b, c.a});
}

void Hud::text(float x, float y, float size, const std::string& str, glm::vec4 color) {
    for (char ch : str) {
        if (ch >= 'a' && ch <= 'z') ch = ch - 'a' + 'A';
        if (ch < 32 || ch > 95) ch = '?';
        const char** glyph = FONT[ch - 32];
        // every '#' becomes a small square
        for (int row = 0; row < 7; row++)
            for (int col = 0; col < 5; col++)
                if (glyph[row][col] == '#')
                    rect(x + col * size, y + row * size, size, size, color);
        x += 6 * size;  // 5 pixels + 1 space
    }
}

float Hud::textWidth(const std::string& str, float size) const {
    return str.size() * 6 * size;
}

void Hud::textCentered(float centerX, float y, float size, const std::string& str, glm::vec4 color) {
    text(centerX - textWidth(str, size) / 2, y, size, str, color);
}

void Hud::end() {
    if (vertices.empty()) return;
    glDisable(GL_DEPTH_TEST);   // always on top
    glEnable(GL_BLEND);         // transparent panels
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.use();
    glUniform2f(glGetUniformLocation(shader.id, "uScreenSize"), (float)width, (float)height);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // changes every frame, so it's uploaded every frame
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, (int)(vertices.size() / 6));

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
