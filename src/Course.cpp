#include "Course.h"

// 4 walls around a rectangle
static void addBorder(Level& level, Wall area) {
    float t = WALL_THICKNESS;
    level.walls.push_back({area.minX - t, area.minZ, area.minX, area.maxZ});          // left
    level.walls.push_back({area.maxX, area.minZ, area.maxX + t, area.maxZ});          // right
    level.walls.push_back({area.minX - t, area.minZ - t, area.maxX + t, area.minZ});  // far
    level.walls.push_back({area.minX - t, area.maxZ, area.maxX + t, area.maxZ + t});  // near
}

std::vector<Level> createLevels() {
    std::vector<Level> levels;

    // hole 1: straight lane, the only way is through the windmill
    Level hole1;
    hole1.name = "THE WINDMILL";
    hole1.par = 2;
    hole1.tee = glm::vec2(0.0f, 1.8f);
    hole1.cup = glm::vec2(0.0f, -7.0f);
    hole1.floors.push_back({-1.5f, -9.0f, 1.5f, 3.0f});
    addBorder(hole1, hole1.floors[0]);
    hole1.hasWindmill = true;
    hole1.windmillPosition = glm::vec3(0.0f, 0.0f, -3.0f);
    hole1.windmillSpeed = 1.0f;
    hole1.lampPosition = glm::vec3(2.3f, 0.0f, -7.5f);
    hole1.center = glm::vec3(0.0f, 0.0f, -3.0f);
    levels.push_back(hole1);

    // hole 2: L-shape turning right, two blocks
    Level hole2;
    hole2.name = "THE DOGLEG";
    hole2.par = 3;
    hole2.tee = glm::vec2(0.0f, 2.0f);
    hole2.cup = glm::vec2(6.2f, -7.25f);
    hole2.floors.push_back({-1.25f, -6.0f, 1.25f, 3.0f});
    hole2.floors.push_back({-1.25f, -8.5f, 7.0f, -6.0f});
    hole2.walls.push_back({-1.45f, -8.5f, -1.25f, 3.0f});  // left
    hole2.walls.push_back({-1.45f, 3.0f, 1.45f, 3.2f});    // behind the tee
    hole2.walls.push_back({1.25f, -5.8f, 1.45f, 3.0f});    // right of the first part
    hole2.walls.push_back({-1.45f, -8.7f, 7.2f, -8.5f});   // top
    hole2.walls.push_back({1.25f, -6.0f, 7.0f, -5.8f});    // bottom of the second part
    hole2.walls.push_back({7.0f, -8.5f, 7.2f, -5.8f});     // end
    hole2.walls.push_back({2.8f, -7.6f, 3.3f, -6.0f});     // block 1
    hole2.walls.push_back({4.6f, -8.5f, 5.1f, -6.9f});     // block 2
    hole2.lampPosition = glm::vec3(7.9f, 0.0f, -9.2f);
    hole2.center = glm::vec3(2.5f, 0.0f, -3.0f);
    levels.push_back(hole2);

    // hole 3: wide lane, fast windmill, posts and a gate
    Level hole3;
    hole3.name = "THE GAUNTLET";
    hole3.par = 3;
    hole3.tee = glm::vec2(0.0f, 2.0f);
    hole3.cup = glm::vec2(0.0f, -9.0f);
    hole3.floors.push_back({-2.0f, -11.0f, 2.0f, 3.0f});
    addBorder(hole3, hole3.floors[0]);
    hole3.walls.push_back({-1.2f, -0.2f, -0.8f, 0.2f});    // posts
    hole3.walls.push_back({0.8f, -0.2f, 1.2f, 0.2f});
    hole3.walls.push_back({-2.0f, -6.4f, -0.45f, -6.2f});  // gate
    hole3.walls.push_back({0.45f, -6.4f, 2.0f, -6.2f});
    hole3.hasWindmill = true;
    hole3.windmillPosition = glm::vec3(0.0f, 0.0f, -3.5f);
    hole3.windmillSpeed = 1.7f;
    hole3.lampPosition = glm::vec3(2.8f, 0.0f, -9.5f);
    hole3.center = glm::vec3(0.0f, 0.0f, -4.0f);
    levels.push_back(hole3);

    return levels;
}
