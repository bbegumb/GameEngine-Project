#include "Rotator.h"

#include <scene/Entity.h>
#include <glm/glm.hpp>

void Rotator::onUpdate(float dt) {
    if (enabled) owner->transform.rotateAroundAxis(axis, dt * speed);
}