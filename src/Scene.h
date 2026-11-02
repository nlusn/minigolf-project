#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

class Scene {
public:
    void init();
    void draw(const Camera& camera, float aspect, glm::vec3 ballPosition);

private:
    void drawObject(const Mesh& mesh, const glm::mat4& model, glm::vec3 color);
    void drawCourse();
    void drawBall(glm::vec3 position);

    Shader shader;
    Mesh cube, sphere, cylinder, cone, disc;
    Mesh grassPlane, groundPlane;
    Mesh longWall, shortWall;
};
