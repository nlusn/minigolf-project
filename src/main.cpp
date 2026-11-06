#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "Ball.h"
#include "Camera.h"
#include "Course.h"
#include "Scene.h"
#include "Windmill.h"

static void onResize(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

enum GameState { AIMING, ROLLING, IN_HOLE, WON };

Camera camera;
Scene scene;
Ball ball;
Windmill windmill;
std::vector<Wall> walls;        // walls of the current hole + windmill base
const float PHYSICS_STEP = 1.0f / 240.0f;
const float MAX_SHOT_SPEED = 9.0f;

std::vector<Level> levels;
int currentLevel = 0;
int scores[3] = {0, 0, 0};      // strokes of each finished hole
float inHoleTimer = 0.0f;

GameState gameState = AIMING;
int strokes = 0;
float aimAngle = 0.0f;        // 0 = towards -z (towards the cup)
float power = 0.0f;           // 0..1
bool charging = false;        // space is held
float powerDirection = 1.0f;  // power bar goes up and down
bool passedTunnel = false;    // went through the windmill this shot

std::string message;          // shown in the title for now, on screen later
float messageTimer = 0.0f;

bool lampOn = true;
bool night = false;
float savedSpeed = 0.0f;      // windmill speed before pausing
bool rightMouseDown = false;
double lastMouseX = 0, lastMouseY = 0;

static void showMessage(const std::string& text, float seconds) {
    message = text;
    messageTimer = seconds;
    std::cout << text << std::endl;
}

static std::string getResultName() {
    if (strokes == 1) {
        if (passedTunnel) return "HOLE IN ONE THROUGH THE WINDMILL!";
        return "HOLE IN ONE!";
    }
    int difference = strokes - levels[currentLevel].par;
    if (difference == -1) return "BIRDIE!";
    if (difference == 0) return "PAR";
    if (difference == 1) return "BOGEY";
    return "+" + std::to_string(difference) + " OVER PAR";
}

static void loadLevel(int index) {
    currentLevel = index;
    const Level& level = levels[index];

    walls = level.walls;
    if (level.hasWindmill) {
        windmill.position = level.windmillPosition;
        windmill.speed = level.windmillSpeed;
        for (const Wall& w : windmill.getWalls()) walls.push_back(w);
    }
    scene.loadLevel(level);

    ball.reset(level.tee);
    strokes = 0;
    aimAngle = 0.0f;
    power = 0.0f;
    charging = false;
    gameState = AIMING;
    camera.yaw = aimAngle;
    showMessage("HOLE " + std::to_string(index + 1) + " - " + level.name + " - PAR " + std::to_string(level.par), 3.0f);
}

static void resetGame() {
    for (int i = 0; i < 3; i++) scores[i] = 0;
    loadLevel(0);
}

static void shoot() {
    charging = false;
    if (gameState != AIMING || power < 0.02f) return;
    // aim angle rotates the direction (0, 0, -1) around y
    glm::vec3 direction(-sin(aimAngle), 0.0f, -cos(aimAngle));
    ball.velocity = direction * (0.5f + power * (MAX_SHOT_SPEED - 0.5f));
    strokes++;
    passedTunnel = false;
    gameState = ROLLING;
}

static void onKey(GLFWwindow* window, int key, int, int action, int) {
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE && charging) shoot();
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, 1);
    if (key == GLFW_KEY_L) lampOn = !lampOn;
    if (key == GLFW_KEY_N) night = !night;
    if (key == GLFW_KEY_SPACE && gameState == AIMING) {
        charging = true;
        power = 0.0f;
        powerDirection = 1.0f;
    }
    if (key == GLFW_KEY_R) resetGame();
    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3) loadLevel(key - GLFW_KEY_1);
    if (key == GLFW_KEY_P) {  // pause / resume the windmill
        if (windmill.speed != 0.0f) {
            savedSpeed = windmill.speed;
            windmill.speed = 0.0f;
        } else {
            windmill.speed = savedSpeed;
        }
    }
    if ((key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) && windmill.speed < 5.0f && windmill.speed != 0.0f)
        windmill.speed += 0.25f;
    if ((key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) && windmill.speed > 0.25f)
        windmill.speed -= 0.25f;
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

// physics runs in small fixed steps so it behaves the same at any fps
static void updatePhysics(float dt) {
    static float timeLeft = 0.0f;
    timeLeft += dt;
    while (timeLeft >= PHYSICS_STEP) {
        timeLeft -= PHYSICS_STEP;
        if (gameState == IN_HOLE || gameState == WON) continue;
        ball.update(PHYSICS_STEP, walls);
        if (levels[currentLevel].hasWindmill) {
            if (windmill.checkBladeHit(ball) && gameState == ROLLING)
                showMessage("BLOCKED BY THE WINDMILL!", 1.2f);
            if (windmill.isInTunnel(ball)) passedTunnel = true;
        }

        // a resting ball pushed by a blade starts rolling again
        if (gameState == AIMING && ball.isMoving()) gameState = ROLLING;
        if (gameState != ROLLING) continue;

        // in the hole: close to the cup centre and not too fast
        glm::vec2 cup = levels[currentLevel].cup;
        glm::vec2 toCup = cup - glm::vec2(ball.position.x, ball.position.z);
        if (glm::length(toCup) < CUP_RADIUS && glm::length(ball.velocity) < 2.5f) {
            ball.velocity = glm::vec3(0.0f);
            ball.position = glm::vec3(cup.x, -0.1f, cup.y);  // drop into the cup
            scores[currentLevel] = strokes;
            showMessage(getResultName(), 3.0f);
            gameState = IN_HOLE;
            inHoleTimer = 2.5f;
        } else if (!ball.isMoving()) {
            gameState = AIMING;
        }
    }
}

// held keys, multiplied by dt so speed doesn't depend on fps
static void processInput(GLFWwindow* window, float dt) {
    if (gameState == AIMING) {
        float turnSpeed = 1.2f;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) turnSpeed = 0.25f;  // fine aim
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            aimAngle += turnSpeed * dt;
            camera.yaw = aimAngle;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            aimAngle -= turnSpeed * dt;
            camera.yaw = aimAngle;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.rotate(1.5f * dt, 0);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.rotate(-1.5f * dt, 0);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.rotate(0, 0.8f * dt);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.rotate(0, -0.8f * dt);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.zoom(-4.0f * dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.zoom(4.0f * dt);

    if (charging) {
        power += powerDirection * dt / 1.2f;
        if (power > 1.0f) { power = 1.0f; powerDirection = -1.0f; }
        if (power < 0.0f) { power = 0.0f; powerDirection = 1.0f; }
    }
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

    scene.init();

    levels = createLevels();
    resetGame();

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
            std::string title = "Mini Golf - FPS " + std::to_string(fps) + " - Strokes " + std::to_string(strokes);
            if (messageTimer > 0.0f) title += " - " + message;
            glfwSetWindowTitle(window, title.c_str());
            frames = 0;
            fpsTimer = 0.0f;
        }

        if (dt > 0.1f) dt = 0.1f;  // e.g. when the window is dragged
        processInput(window, dt);
        windmill.update(dt);
        updatePhysics(dt);
        if (messageTimer > 0.0f) messageTimer -= dt;
        if (gameState == IN_HOLE) {
            inHoleTimer -= dt;
            if (inHoleTimer <= 0.0f) {
                if (currentLevel < (int)levels.size() - 1) {
                    loadLevel(currentLevel + 1);
                } else {
                    gameState = WON;
                    int total = scores[0] + scores[1] + scores[2];
                    showMessage("COURSE COMPLETE! TOTAL " + std::to_string(total) + " - PRESS R TO PLAY AGAIN", 1000.0f);
                }
            }
        }
        camera.target = glm::vec3(ball.position.x, 0.1f, ball.position.z);

        if (night) glClearColor(0.02f, 0.03f, 0.1f, 1.0f);
        else glClearColor(0.53f, 0.75f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width == 0 || height == 0) {  // minimized
            glfwPollEvents();
            continue;
        }

        SceneState state;
        state.lampOn = lampOn;
        state.night = night;
        glm::vec3 center = levels[currentLevel].center;
        state.lanternPosition = center + glm::vec3(2.6f * cos(now * 0.6f), 1.2f, 2.6f * sin(now * 0.6f));
        state.showAim = (gameState == AIMING);
        state.aimAngle = aimAngle;
        state.power = power;
        state.time = now;
        scene.draw(camera, (float)width / height, levels[currentLevel], ball.position, windmill, state);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
