#include "Windmill.h"
#include <cmath>

void Windmill::update(float dt) {
    angle += speed * dt;
    if (angle > 6.2831853f) angle -= 6.2831853f;
}

std::vector<Wall> Windmill::getWalls() const {
    float x = position.x, z = position.z;
    std::vector<Wall> walls;
    walls.push_back({x - BASE_HALF_WIDTH, z - BASE_HALF_DEPTH, x - TUNNEL_HALF_WIDTH, z + BASE_HALF_DEPTH});
    walls.push_back({x + TUNNEL_HALF_WIDTH, z - BASE_HALF_DEPTH, x + BASE_HALF_WIDTH, z + BASE_HALF_DEPTH});
    return walls;
}

bool Windmill::isInTunnel(const Ball& ball) const {
    return fabs(ball.position.x - position.x) < TUNNEL_HALF_WIDTH &&
           fabs(ball.position.z - position.z) < BASE_HALF_DEPTH;
}

bool Windmill::checkBladeHit(Ball& ball) const {
    // blades spin in the plane z = bladeZ, only check when the ball touches it
    float bladeZ = position.z + HUB_FRONT;
    if (fabs(ball.position.z - bladeZ) > ball.radius + 0.02f) return false;

    for (int i = 0; i < 4; i++) {
        float a = angle + i * 1.5707963f;
        float dirX = -sin(a);   // blade direction in the x-y plane
        float dirY = cos(a);
        if (dirY >= 0.0f) continue;  // pointing up, can't reach the ball

        // go down the blade until we are at the ball's height
        float lengthToBall = (HUB_HEIGHT - ball.position.y) / -dirY;
        if (lengthToBall > BLADE_LENGTH) continue;

        float bladeX = position.x + dirX * lengthToBall;
        if (fabs(ball.position.x - bladeX) < BLADE_HALF_WIDTH + ball.radius) {
            // push the ball back to the side it came from
            float side;
            if (ball.velocity.z < 0.0f) side = 1.0f;
            else if (ball.velocity.z > 0.0f) side = -1.0f;
            else side = (ball.position.z > bladeZ) ? 1.0f : -1.0f;
            ball.position.z = bladeZ + side * (ball.radius + 0.03f);
            ball.velocity.z = side * (fabs(ball.velocity.z) * 0.6f + 0.3f);
            return true;
        }
    }
    return false;
}
