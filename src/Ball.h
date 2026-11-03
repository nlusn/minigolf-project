#pragma once
#include "Course.h"
#include <glm/glm.hpp>
#include <vector>

class Ball {
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);   // y is always 0, the ball stays on the ground
    float radius = BALL_RADIUS;

    void reset(glm::vec2 xz);
    bool isMoving() const;
    // returns true if the ball hit a wall
    bool update(float dt, const std::vector<Wall>& walls);
};
