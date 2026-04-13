#include "Rotator.h"

#include <scene/Entity.h>
#include <glm/glm.hpp>

void Rotator::onUpdate(float dt) {
	glm::vec3 rot = owner->getTransform().getRotation();
	rot.z += dt;

	owner->getTransform().setRotation(rot);
}