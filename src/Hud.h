#pragma once
#include "Shader.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

// 2D rectangles and text on top of the 3D scene, one draw call per frame.
// coordinates in pixels, (0, 0) = top-left corner
class Hud {
public:
    void init();
    void begin(int screenWidth, int screenHeight);
    void rect(float x, float y, float w, float h, glm::vec4 color);
    // size = size of one font pixel
    void text(float x, float y, float size, const std::string& str, glm::vec4 color);
    void textCentered(float centerX, float y, float size, const std::string& str, glm::vec4 color);
    float textWidth(const std::string& str, float size) const;
    void end();

private:
    Shader shader;
    unsigned int vao = 0, vbo = 0;
    std::vector<float> vertices;   // x, y, r, g, b, a
    int width = 1, height = 1;
};
