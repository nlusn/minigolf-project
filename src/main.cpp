#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include "Camera.h"
#include "Course.h"
#include "Scene.h"

static void onResize(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

Camera camera;
bool rightMouseDown = false;
double lastMouseX = 0, lastMouseY = 0;

static void onKey(GLFWwindow* window, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, 1);
}

static void onMouseButton(GLFWwindow*, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) rightMouseDown = (action == GLFW_PRESS);
}

static void onCursor(GLFWwindow*, double x, double y) {
    if (rightMouseDown)
        camera.rotate(-(float)(x - lastMouseX) * 0.005f, (float)(y - lastMouseY) * 0.005f);
    lastMouseX = x;
    lastMouseY = y;
}

static void onScroll(GLFWwindow*, double, double yOffset) {
    camera.zoom(-(float)yOffset * 0.5f);
}

// held keys, multiplied by dt so speed doesn't depend on fps
static void processInput(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.rotate(1.5f * dt, 0);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.rotate(-1.5f * dt, 0);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.rotate(0, 0.8f * dt);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.rotate(0, -0.8f * dt);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.zoom(-4.0f * dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.zoom(4.0f * dt);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialise GLFW\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // macos requirement

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Mini Golf", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create an OpenGL 3.3 window\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // vsync

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to load OpenGL functions\n";
        return 1;
    }

    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSetKeyCallback(window, onKey);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onCursor);
    glfwSetScrollCallback(window, onScroll);
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    Scene scene;
    scene.init();

    glm::vec3 ballPosition(TEE_POSITION.x, BALL_RADIUS, TEE_POSITION.y);
    camera.target = ballPosition;

    float lastTime = (float)glfwGetTime();
    float fpsTimer = 0.0f;
    int frames = 0;

    while (!glfwWindowShouldClose(window)) {
        float now = (float)glfwGetTime();
        float dt = now - lastTime;
        lastTime = now;

        frames++;
        fpsTimer += dt;
        if (fpsTimer >= 0.5f) {
            int fps = (int)(frames / fpsTimer);
            std::string title = "Mini Golf - FPS " + std::to_string(fps);
            glfwSetWindowTitle(window, title.c_str());
            frames = 0;
            fpsTimer = 0.0f;
        }

        processInput(window, dt);

        glClearColor(0.53f, 0.75f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width == 0 || height == 0) {  // minimized
            glfwPollEvents();
            continue;
        }

        scene.draw(camera, (float)width / height, ballPosition);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
