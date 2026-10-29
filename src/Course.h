#pragma once
#include <glm/glm.hpp>

// course goes along z: tee at z = +1.8, cup at z = -7, grass at y = 0
const float COURSE_MIN_X = -1.5f, COURSE_MAX_X = 1.5f;
const float COURSE_MIN_Z = -9.0f, COURSE_MAX_Z = 3.0f;
const float WALL_THICKNESS = 0.2f;
const float WALL_HEIGHT = 0.25f;

const glm::vec2 TEE_POSITION(0.0f, 1.8f);   // (x, z)
const glm::vec2 CUP_POSITION(0.0f, -7.0f);
const float CUP_RADIUS = 0.17f;
const float BALL_RADIUS = 0.1f;
const int PAR = 2;

struct Wall {
    float minX, minZ, maxX, maxZ;
};
