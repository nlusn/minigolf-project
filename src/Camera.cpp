#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void Camera::rotate(float deltaYaw, float deltaPitch) {
    yaw += deltaYaw;
    pitch += deltaPitch;
    if (pitch < 0.1f) pitch = 0.1f;
    if (pitch > 1.5f) pitch = 1.5f;
}

void Camera::zoom(float amount) {
    distance += amount;
    if (distance < 1.5f) distance = 1.5f;
    if (distance > 20.0f) distance = 20.0f;
}

glm::vec3 Camera::getPosition() const {
    float x = distance * cos(pitch) * sin(yaw);
    float y = distance * sin(pitch);
    float z = distance * cos(pitch) * cos(yaw);
    return target + glm::vec3(x, y, z);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(getPosition(), target, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::getProjectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(50.0f), aspect, 0.1f, 100.0f);
}
