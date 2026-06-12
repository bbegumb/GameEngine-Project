#include "Rotator.h"

#include <scene/Entity.h>

void Rotator::onUpdate(float dt) {
    if (enabled) owner->transform.rotateAroundAxis(axis, dt * speed);
}