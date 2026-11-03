#include "Ball.h"
#include <cmath>

const float FRICTION = 1.2f;
const float BOUNCINESS = 0.75f;   // 1 = perfect bounce
const float STOP_SPEED = 0.05f;

void Ball::reset(glm::vec2 xz) {
    position = glm::vec3(xz.x, radius, xz.y);
    velocity = glm::vec3(0.0f);
}

bool Ball::isMoving() const {
    return glm::length(velocity) > 0.0f;
}

bool Ball::update(float dt, const std::vector<Wall>& walls) {
    float speed = glm::length(velocity);
    if (speed == 0.0f) return false;

    // friction slows the ball down but keeps the direction
    float newSpeed = speed - (FRICTION + 0.3f * speed) * dt;
    if (newSpeed < STOP_SPEED) {
        velocity = glm::vec3(0.0f);
        return false;
    }
    velocity = velocity / speed * newSpeed;
    position += velocity * dt;

    bool hit = false;
    for (const Wall& w : walls) {
        // closest point of the wall rectangle to the ball centre
        float closestX = glm::clamp(position.x, w.minX, w.maxX);
        float closestZ = glm::clamp(position.z, w.minZ, w.maxZ);
        float dx = position.x - closestX;
        float dz = position.z - closestZ;
        float distance = sqrt(dx * dx + dz * dz);
        if (distance >= radius || distance == 0.0f) continue;

        // on a straight wall the normal is (1,0,0) or (0,0,1), so only vx or vz flips
        glm::vec3 normal(dx / distance, 0.0f, dz / distance);
        position += normal * (radius - distance);

        // reflection: v = v - (1 + bounciness) * (v . n) * n
        float vn = glm::dot(velocity, normal);
        if (vn < 0.0f) {
            velocity -= (1.0f + BOUNCINESS) * vn * normal;
            hit = true;
        }
    }
    return hit;
}
