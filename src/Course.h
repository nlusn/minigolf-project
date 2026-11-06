#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

// y is up, grass at y = 0, positions on the ground are (x, z)
const float WALL_THICKNESS = 0.2f;
const float WALL_HEIGHT = 0.25f;
const float CUP_RADIUS = 0.17f;
const float BALL_RADIUS = 0.1f;

// rectangle seen from above, used for walls and grass areas
struct Wall {
    float minX, minZ, maxX, maxZ;
};

struct Level {
    std::string name;
    int par = 2;
    glm::vec2 tee;
    glm::vec2 cup;
    std::vector<Wall> floors;
    std::vector<Wall> walls;
    bool hasWindmill = false;
    glm::vec3 windmillPosition = glm::vec3(0.0f);
    float windmillSpeed = 1.0f;
    glm::vec3 lampPosition;
    glm::vec3 center;   // the lantern flies around it
};

std::vector<Level> createLevels();
