#include "Scene.h"
#include "Course.h"
#include <glm/gtc/matrix_transform.hpp>

using glm::mat4;
using glm::vec3;

void Scene::init() {
    shader.load("shaders/basic.vert", "shaders/basic.frag");

    cube = makeBox(1, 1, 1);
    sphere = makeSphere(1.0f, 24, 12);
    cylinder = makeCylinder(1.0f, 1.0f, 20);
    cone = makeCylinder(1.0f, 0.0f, 20);
    disc = makeDisc(1.0f, 24);

    float length = COURSE_MAX_Z - COURSE_MIN_Z;
    float width = COURSE_MAX_X - COURSE_MIN_X;
    grassPlane = makePlane(width, length);
    groundPlane = makePlane(80.0f, 80.0f);
    // walls go from y = -0.2 (ground) up to WALL_HEIGHT
    longWall = makeBox(WALL_THICKNESS, WALL_HEIGHT + 0.2f, length);
    shortWall = makeBox(width + 2 * WALL_THICKNESS, WALL_HEIGHT + 0.2f, WALL_THICKNESS);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void Scene::drawObject(const Mesh& mesh, const mat4& model, vec3 color, bool glow) {
    shader.setMat4("uModel", model);
    // keeps normals correct when the model is scaled
    shader.setMat3("uNormalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
    shader.setVec3("uColor", color);
    shader.setInt("uGlow", glow ? 1 : 0);
    mesh.draw();
}

void Scene::draw(const Camera& camera, float aspect, vec3 ballPosition, const SceneState& state) {
    shader.use();
    shader.setMat4("uView", camera.getViewMatrix());
    shader.setMat4("uProjection", camera.getProjectionMatrix(aspect));
    shader.setVec3("uViewPosition", camera.getPosition());

    // sun, weak blue moonlight at night
    shader.setVec3("uSunDirection", glm::normalize(vec3(0.4f, 1.0f, 0.3f)));
    if (state.night) {
        shader.setVec3("uSunColor", vec3(0.15f, 0.18f, 0.3f));
        shader.setVec3("uAmbient", vec3(0.05f, 0.05f, 0.08f));
    } else {
        shader.setVec3("uSunColor", vec3(1.0f, 0.95f, 0.85f));
        shader.setVec3("uAmbient", vec3(0.3f, 0.3f, 0.35f));
    }

    // lamp post and the moving lantern
    float lampStrength = state.lampOn ? (state.night ? 1.5f : 0.6f) : 0.0f;
    shader.setVec3("uLampPosition", LAMP_POSITION + vec3(0, 1.7f, 0));
    shader.setVec3("uLampColor", vec3(1.0f, 0.75f, 0.4f) * lampStrength);
    shader.setVec3("uLanternPosition", state.lanternPosition);
    shader.setVec3("uLanternColor", vec3(0.4f, 0.7f, 1.0f) * lampStrength);

    drawCourse();
    drawLamps(state);
    drawBall(ballPosition);
}

void Scene::drawCourse() {
    float centerX = (COURSE_MIN_X + COURSE_MAX_X) / 2;
    float centerZ = (COURSE_MIN_Z + COURSE_MAX_Z) / 2;
    float wallY = (WALL_HEIGHT - 0.2f) / 2;
    float t = WALL_THICKNESS / 2;

    drawObject(groundPlane, glm::translate(mat4(1.0f), vec3(0, -0.2f, centerZ)), vec3(0.25f, 0.45f, 0.2f));
    drawObject(grassPlane, glm::translate(mat4(1.0f), vec3(centerX, 0, centerZ)), vec3(0.2f, 0.7f, 0.2f));

    vec3 wood(0.55f, 0.35f, 0.2f);
    drawObject(longWall, glm::translate(mat4(1.0f), vec3(COURSE_MIN_X - t, wallY, centerZ)), wood);
    drawObject(longWall, glm::translate(mat4(1.0f), vec3(COURSE_MAX_X + t, wallY, centerZ)), wood);
    drawObject(shortWall, glm::translate(mat4(1.0f), vec3(centerX, wallY, COURSE_MIN_Z - t)), wood);
    drawObject(shortWall, glm::translate(mat4(1.0f), vec3(centerX, wallY, COURSE_MAX_Z + t)), wood);

    // cup is a dark disc just above the grass
    mat4 cup = glm::translate(mat4(1.0f), vec3(CUP_POSITION.x, 0.002f, CUP_POSITION.y));
    drawObject(disc, glm::scale(cup, vec3(CUP_RADIUS, 1, CUP_RADIUS)), vec3(0.03f));
}

void Scene::drawLamps(const SceneState& state) {
    mat4 pole = glm::translate(mat4(1.0f), LAMP_POSITION + vec3(0, -0.2f, 0));
    drawObject(cylinder, glm::scale(pole, vec3(0.05f, 1.8f, 0.05f)), vec3(0.15f));

    vec3 bulbColor = state.lampOn ? vec3(1.0f, 0.85f, 0.5f) : vec3(0.3f);
    mat4 bulb = glm::translate(mat4(1.0f), LAMP_POSITION + vec3(0, 1.7f, 0));
    drawObject(sphere, glm::scale(bulb, vec3(0.12f)), bulbColor, state.lampOn);

    vec3 lanternColor = state.lampOn ? vec3(0.5f, 0.8f, 1.0f) : vec3(0.3f);
    mat4 lantern = glm::translate(mat4(1.0f), state.lanternPosition);
    drawObject(sphere, glm::scale(lantern, vec3(0.08f)), lanternColor, state.lampOn);
}

void Scene::drawBall(vec3 position) {
    mat4 model = glm::translate(mat4(1.0f), position);
    drawObject(sphere, glm::scale(model, vec3(BALL_RADIUS)), vec3(1.0f));
}
