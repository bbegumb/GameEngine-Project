#include "Rotator.h"

#include <scene/Entity.h>
#include <glm/glm.hpp>

void Rotator::onUpdate(float dt) {
	timeElapsed += dt;

	if (timeElapsed < 2.0f) {
		x += dt;
	}
	else if (timeElapsed < 4.0f) {
		y += dt;
	}
	else if (timeElapsed < 6.0f) {
		z += dt;
	}
	else timeElapsed = 0.0f;

	x = (x > 360.0f) ? x - 360.0f : x;
	y = (y > 360.0f) ? y - 360.0f : y;
	z = (z > 360.0f) ? z - 360.0f : z;

	owner->getTransform().setRotation(glm::vec3(x, y, z));
}