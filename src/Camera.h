#pragma once
#include <glm/glm.hpp>

// orbit camera around target
class Camera {
public:
    glm::vec3 target = glm::vec3(0.0f);
    float yaw = 0.0f;
    float pitch = 0.45f;
    float distance = 5.0f;

    void rotate(float deltaYaw, float deltaPitch);
    void zoom(float amount);

    glm::vec3 getPosition() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;
};
