#pragma once
#include "Camera.h"
#include "Course.h"
#include "Mesh.h"
#include "Shader.h"
#include "Windmill.h"
#include <glm/glm.hpp>
#include <vector>

struct SceneState {
    bool lampOn = true;
    bool night = false;
    glm::vec3 lanternPosition = glm::vec3(0.0f);
    bool showAim = false;
    float aimAngle = 0.0f;
    float power = 0.0f;   // 0..1
    float time = 0.0f;
};

class Scene {
public:
    void init();
    // builds the grass and wall meshes of a hole, called once when the hole starts
    void loadLevel(const Level& level);
    void draw(const Camera& camera, float aspect, const Level& level, glm::vec3 ballPosition,
              const Windmill& windmill, const SceneState& state);

private:
    // glow = no lighting, used for light bulbs
    void drawObject(const Mesh& mesh, const glm::mat4& model, glm::vec3 color, unsigned int texture = 0, bool glow = false);
    void drawCourse(const Level& level);
    void drawLamps(const Level& level, const SceneState& state);
    void drawFlag(const Level& level, const SceneState& state);
    void drawWindmill(const Windmill& windmill);
    void drawBall(glm::vec3 position);
    void drawAimLine(glm::vec3 ballPosition, const SceneState& state);

    Shader shader;
    Mesh cube, sphere, cylinder, cone, disc;
    Mesh groundPlane;

    struct PlacedMesh {
        Mesh mesh;
        glm::vec3 position;
    };
    std::vector<PlacedMesh> floorPieces;
    std::vector<PlacedMesh> wallPieces;
    Mesh baseSide, baseTop;
    unsigned int grassTexture = 0, woodTexture = 0, brickTexture = 0;
};
