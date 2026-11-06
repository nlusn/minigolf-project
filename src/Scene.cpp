#include "Scene.h"
#include "Course.h"
#include "Texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

using glm::mat4;
using glm::vec3;

void Scene::init() {
    shader.load("shaders/basic.vert", "shaders/basic.frag");

    cube = makeBox(1, 1, 1);
    sphere = makeSphere(1.0f, 24, 12);
    cylinder = makeCylinder(1.0f, 1.0f, 20);
    cone = makeCylinder(1.0f, 0.0f, 20);
    disc = makeDisc(1.0f, 24);

    // last number = texture tile size, so textures repeat instead of stretching
    groundPlane = makePlane(80.0f, 80.0f, 2.5f);

    grassTexture = makeGrassTexture();
    woodTexture = makeWoodTexture();
    brickTexture = makeBrickTexture();

    baseSide = makeBox(BASE_HALF_WIDTH - TUNNEL_HALF_WIDTH, BASE_HEIGHT, 2 * BASE_HALF_DEPTH, 0.5f);
    baseTop = makeBox(2 * TUNNEL_HALF_WIDTH, BASE_HEIGHT - TUNNEL_HEIGHT, 2 * BASE_HALF_DEPTH, 0.5f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void Scene::loadLevel(const Level& level) {
    for (PlacedMesh& p : floorPieces) p.mesh.destroy();
    for (PlacedMesh& p : wallPieces) p.mesh.destroy();
    floorPieces.clear();
    wallPieces.clear();

    for (const Wall& f : level.floors) {
        float w = f.maxX - f.minX, d = f.maxZ - f.minZ;
        floorPieces.push_back({makePlane(w, d, 1.0f), vec3(f.minX + w / 2, 0.0f, f.minZ + d / 2)});
    }
    // walls go from y = -0.2 (ground) up to WALL_HEIGHT
    float wallH = WALL_HEIGHT + 0.2f;
    for (const Wall& wl : level.walls) {
        float w = wl.maxX - wl.minX, d = wl.maxZ - wl.minZ;
        wallPieces.push_back({makeBox(w, wallH, d, 0.6f), vec3(wl.minX + w / 2, WALL_HEIGHT - wallH / 2, wl.minZ + d / 2)});
    }
}

void Scene::drawObject(const Mesh& mesh, const mat4& model, vec3 color, unsigned int texture, bool glow) {
    shader.setMat4("uModel", model);
    // keeps normals correct when the model is scaled
    shader.setMat3("uNormalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
    shader.setVec3("uColor", color);
    shader.setInt("uGlow", glow ? 1 : 0);
    shader.setInt("uUseTexture", texture != 0 ? 1 : 0);
    if (texture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
    mesh.draw();
}

void Scene::draw(const Camera& camera, float aspect, const Level& level, vec3 ballPosition,
                 const Windmill& windmill, const SceneState& state) {
    shader.use();
    shader.setMat4("uView", camera.getViewMatrix());
    shader.setMat4("uProjection", camera.getProjectionMatrix(aspect));
    shader.setVec3("uViewPosition", camera.getPosition());
    shader.setInt("uTexture", 0);

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
    shader.setVec3("uLampPosition", level.lampPosition + vec3(0, 1.7f, 0));
    shader.setVec3("uLampColor", vec3(1.0f, 0.75f, 0.4f) * lampStrength);
    shader.setVec3("uLanternPosition", state.lanternPosition);
    shader.setVec3("uLanternColor", vec3(0.4f, 0.7f, 1.0f) * lampStrength);

    drawCourse(level);
    if (level.hasWindmill) drawWindmill(windmill);
    drawFlag(level, state);
    drawLamps(level, state);
    drawBall(ballPosition);
    if (state.showAim) drawAimLine(ballPosition, state);
}

void Scene::drawCourse(const Level& level) {
    drawObject(groundPlane, glm::translate(mat4(1.0f), vec3(level.center.x, -0.2f, level.center.z)), vec3(0.55f, 0.6f, 0.4f), grassTexture);

    for (const PlacedMesh& p : floorPieces)
        drawObject(p.mesh, glm::translate(mat4(1.0f), p.position), vec3(1.0f), grassTexture);
    for (const PlacedMesh& p : wallPieces)
        drawObject(p.mesh, glm::translate(mat4(1.0f), p.position), vec3(1.0f), woodTexture);

    // cup is a dark disc just above the grass
    mat4 cup = glm::translate(mat4(1.0f), vec3(level.cup.x, 0.002f, level.cup.y));
    drawObject(disc, glm::scale(cup, vec3(CUP_RADIUS, 1, CUP_RADIUS)), vec3(0.03f));
}

// hierarchy: windmill -> base, tower -> roof
//            windmill -> rotor (spins) -> hub, blade 0..3 -> spar, sail
void Scene::drawWindmill(const Windmill& windmill) {
    mat4 windmillMatrix = glm::translate(mat4(1.0f), windmill.position);

    float sideX = (TUNNEL_HALF_WIDTH + BASE_HALF_WIDTH) / 2;
    drawObject(baseSide, glm::translate(windmillMatrix, vec3(-sideX, BASE_HEIGHT / 2, 0)), vec3(1.0f), brickTexture);
    drawObject(baseSide, glm::translate(windmillMatrix, vec3(sideX, BASE_HEIGHT / 2, 0)), vec3(1.0f), brickTexture);
    drawObject(baseTop, glm::translate(windmillMatrix, vec3(0, (BASE_HEIGHT + TUNNEL_HEIGHT) / 2, 0)), vec3(1.0f), brickTexture);
    mat4 tunnelFloor = glm::translate(windmillMatrix, vec3(0, 0.003f, 0));
    drawObject(cube, glm::scale(tunnelFloor, vec3(2 * TUNNEL_HALF_WIDTH, 0.004f, 2 * BASE_HALF_DEPTH)), vec3(0.15f, 0.12f, 0.1f));

    mat4 towerMatrix = glm::translate(windmillMatrix, vec3(0, BASE_HEIGHT, 0));
    drawObject(cylinder, glm::scale(towerMatrix, vec3(0.7f, 1.5f, 0.7f)), vec3(1.0f, 0.95f, 0.9f), brickTexture);
    mat4 roofMatrix = glm::translate(towerMatrix, vec3(0, 1.5f, 0));
    drawObject(cone, glm::scale(roofMatrix, vec3(0.85f, 0.75f, 0.85f)), vec3(0.65f, 0.18f, 0.12f));

    mat4 rotorMatrix = glm::translate(windmillMatrix, vec3(0, HUB_HEIGHT, HUB_FRONT));
    rotorMatrix = glm::rotate(rotorMatrix, windmill.angle, vec3(0, 0, 1));

    mat4 hubMatrix = glm::translate(rotorMatrix, vec3(0, 0, -0.35f));
    hubMatrix = glm::rotate(hubMatrix, glm::radians(90.0f), vec3(1, 0, 0));
    drawObject(cylinder, glm::scale(hubMatrix, vec3(0.14f, 0.42f, 0.14f)), vec3(0.3f, 0.2f, 0.12f));

    float sailStart = 0.42f;
    float sailLength = BLADE_LENGTH - sailStart;
    for (int i = 0; i < 4; i++) {
        mat4 bladeMatrix = glm::rotate(rotorMatrix, glm::radians(90.0f * i), vec3(0, 0, 1));
        mat4 spar = glm::translate(bladeMatrix, vec3(0, BLADE_LENGTH / 2, 0));
        drawObject(cube, glm::scale(spar, vec3(0.07f, BLADE_LENGTH, 0.06f)), vec3(1.0f), woodTexture);
        mat4 sail = glm::translate(bladeMatrix, vec3(0, sailStart + sailLength / 2, 0));
        drawObject(cube, glm::scale(sail, vec3(2 * BLADE_HALF_WIDTH, sailLength, 0.03f)), vec3(0.92f, 0.88f, 0.78f));
    }
}

void Scene::drawFlag(const Level& level, const SceneState& state) {
    // the flag is a child of the pole: its matrix starts from the pole's matrix
    mat4 poleMatrix = glm::translate(mat4(1.0f), vec3(level.cup.x, 0, level.cup.y));
    drawObject(cylinder, glm::scale(poleMatrix, vec3(0.015f, 1.3f, 0.015f)), vec3(0.95f));

    mat4 flagMatrix = glm::translate(poleMatrix, vec3(0, 1.18f, 0));
    flagMatrix = glm::rotate(flagMatrix, 0.25f * sinf(state.time * 2.0f), vec3(0, 1, 0));  // waving
    mat4 flag = glm::translate(flagMatrix, vec3(0.2f, 0, 0));
    drawObject(cube, glm::scale(flag, vec3(0.4f, 0.24f, 0.01f)), vec3(0.9f, 0.1f, 0.1f));
}

void Scene::drawLamps(const Level& level, const SceneState& state) {
    mat4 pole = glm::translate(mat4(1.0f), level.lampPosition + vec3(0, -0.2f, 0));
    drawObject(cylinder, glm::scale(pole, vec3(0.05f, 1.8f, 0.05f)), vec3(0.15f));

    vec3 bulbColor = state.lampOn ? vec3(1.0f, 0.85f, 0.5f) : vec3(0.3f);
    mat4 bulb = glm::translate(mat4(1.0f), level.lampPosition + vec3(0, 1.7f, 0));
    drawObject(sphere, glm::scale(bulb, vec3(0.12f)), bulbColor, 0, state.lampOn);

    vec3 lanternColor = state.lampOn ? vec3(0.5f, 0.8f, 1.0f) : vec3(0.3f);
    mat4 lantern = glm::translate(mat4(1.0f), state.lanternPosition);
    drawObject(sphere, glm::scale(lantern, vec3(0.08f)), lanternColor, 0, state.lampOn);
}

void Scene::drawBall(vec3 position) {
    mat4 model = glm::translate(mat4(1.0f), position);
    drawObject(sphere, glm::scale(model, vec3(BALL_RADIUS)), vec3(1.0f));
}

void Scene::drawAimLine(vec3 ballPosition, const SceneState& state) {
    // thin line on the ground, longer and redder with more power
    float length = 0.5f + state.power * 2.5f;
    vec3 color = glm::mix(vec3(1.0f, 1.0f, 0.3f), vec3(1.0f, 0.1f, 0.1f), state.power);
    mat4 model = glm::translate(mat4(1.0f), vec3(ballPosition.x, 0.01f, ballPosition.z));
    model = glm::rotate(model, state.aimAngle, vec3(0, 1, 0));
    model = glm::translate(model, vec3(0, 0, -(BALL_RADIUS + length / 2)));
    drawObject(cube, glm::scale(model, vec3(0.03f, 0.005f, length)), color, 0, true);
}
