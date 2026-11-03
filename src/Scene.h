#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

struct SceneState {
    bool lampOn = true;
    bool night = false;
    glm::vec3 lanternPosition = glm::vec3(0.0f);
    bool showAim = false;
    float aimAngle = 0.0f;
    float power = 0.0f;   // 0..1
};

class Scene {
public:
    void init();
    void draw(const Camera& camera, float aspect, glm::vec3 ballPosition, const SceneState& state);

private:
    // glow = no lighting, used for light bulbs
    void drawObject(const Mesh& mesh, const glm::mat4& model, glm::vec3 color, unsigned int texture = 0, bool glow = false);
    void drawCourse();
    void drawLamps(const SceneState& state);
    void drawBall(glm::vec3 position);
    void drawAimLine(glm::vec3 ballPosition, const SceneState& state);

    Shader shader;
    Mesh cube, sphere, cylinder, cone, disc;
    Mesh grassPlane, groundPlane;
    Mesh longWall, shortWall;
    unsigned int grassTexture = 0, woodTexture = 0;
};
