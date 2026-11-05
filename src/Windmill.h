#pragma once
#include "Ball.h"
#include "Course.h"
#include <glm/glm.hpp>
#include <vector>

// sizes relative to the windmill position, it faces +z (towards the tee)
const float BASE_HALF_WIDTH = 1.5f;
const float BASE_HALF_DEPTH = 0.8f;
const float BASE_HEIGHT = 1.0f;
const float TUNNEL_HALF_WIDTH = 0.3f;
const float TUNNEL_HEIGHT = 0.45f;
const float HUB_HEIGHT = 1.75f;
const float HUB_FRONT = 0.95f;      // z of the blades, in front of the base
const float BLADE_LENGTH = 1.68f;   // reaches down to the tunnel
const float BLADE_HALF_WIDTH = 0.25f;

class Windmill {
public:
    glm::vec3 position = glm::vec3(0.0f);
    float angle = 0.0f;   // radians
    float speed = 1.0f;   // radians per second

    void update(float dt);
    // solid parts of the base, left and right of the tunnel
    std::vector<Wall> getWalls() const;
    // bounces the ball back if a blade is in the way, returns true on a hit
    bool checkBladeHit(Ball& ball) const;
    bool isInTunnel(const Ball& ball) const;
};
