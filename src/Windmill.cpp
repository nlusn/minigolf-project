#include "Windmill.h"

void Windmill::update(float dt) {
    angle += speed * dt;
    if (angle > 6.2831853f) angle -= 6.2831853f;
}
