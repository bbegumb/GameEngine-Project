#include "Entity.h"

#include <scene/components/TransformComponent.h>

TransformComponent& Entity::getTransform() {
	return *getComponent<TransformComponent>();
}

const TransformComponent& Entity::getTransform() const {
	return *getComponent<TransformComponent>();
}