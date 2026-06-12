#include "TransformController.h"

glm::vec3 TransformController::getPosition(Entity* entity) {
    if (!entity) return glm::vec3(0.0f);
    return entity->getTransform().getPosition();
}

glm::vec3 TransformController::getRotation(Entity* entity) {
    if (!entity) return glm::vec3(0.0f);
    return entity->getTransform().getEulerRotation();
}

glm::vec3 TransformController::getScale(Entity* entity) {
    if (!entity) return glm::vec3(1.0f);
    return entity->getTransform().getScale();
}

void TransformController::setPosition(Entity* entity, const glm::vec3& position) {
    if (!entity) return;
    entity->getTransform().setPosition(position);
}

void TransformController::setRotation(Entity* entity, const glm::vec3& rotation) {
    if (!entity) return;
    entity->getTransform().setRotation(rotation);
}

void TransformController::setScale(Entity* entity, const glm::vec3& scale) {
    if (!entity) return;
    entity->getTransform().setScale(scale);
}

