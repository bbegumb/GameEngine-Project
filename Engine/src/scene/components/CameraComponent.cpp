#include "CameraComponent.h"

#include <scene/Entity.h>
#include <scene/components/TransformComponent.h>

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 CameraComponent::getViewMatrix() const {
    const auto& transform = owner->getTransform();

    glm::mat4 view = glm::mat4(1.0f);
    glm::vec3 rotation = transform.getRotation();
    glm::vec3 pos = transform.getPosition();
    view = glm::rotate(view, -rotation.z, glm::vec3(0, 0, 1));
    view = glm::rotate(view, -rotation.y, glm::vec3(0, 1, 0));
    view = glm::rotate(view, -rotation.x, glm::vec3(1, 0, 0));
    view = glm::translate(view, -pos);

    return view;
}

glm::mat4 CameraComponent::getProjectionMatrix() const {
	return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}