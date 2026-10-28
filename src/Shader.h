#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    unsigned int id = 0;

    void load(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;

    // Helpers to send uniforms to the shader
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVec3(const char* name, const glm::vec3& value) const;
    void setVec4(const char* name, const glm::vec4& value) const;
    void setMat3(const char* name, const glm::mat3& value) const;
    void setMat4(const char* name, const glm::mat4& value) const;
};
