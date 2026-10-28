#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cout << "ERROR: cannot open " << path << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static unsigned int compileShader(GLenum type, const std::string& source, const std::string& path) {
    unsigned int shader = glCreateShader(type);
    const char* code = source.c_str();
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        std::cout << "ERROR compiling " << path << ":\n" << log << std::endl;
    }
    return shader;
}

void Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {
    unsigned int vs = compileShader(GL_VERTEX_SHADER, readFile(vertexPath), vertexPath);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, readFile(fragmentPath), fragmentPath);

    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(id, 1024, nullptr, log);
        std::cout << "ERROR linking shader program:\n" << log << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::use() const { glUseProgram(id); }

void Shader::setInt(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(id, name), value);
}
void Shader::setFloat(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(id, name), value);
}
void Shader::setVec3(const char* name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(id, name), 1, glm::value_ptr(value));
}
void Shader::setVec4(const char* name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(id, name), 1, glm::value_ptr(value));
}
void Shader::setMat3(const char* name, const glm::mat3& value) const {
    glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setMat4(const char* name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(value));
}
