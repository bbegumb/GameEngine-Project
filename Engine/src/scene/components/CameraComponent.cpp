#include "CameraComponent.h"

#include <persistance/ComponentFactory.h>
#include <scene/Entity.h>
#include <scene/components/TransformComponent.h>

#include <glm/gtc/matrix_transform.hpp>

REGISTER(CameraComponent);

glm::mat4 CameraComponent::getViewMatrix() const {
    glm::mat4 world = owner->getTransform().getMatrix();

    glm::vec3 pos = glm::vec3(world[3]);
    glm::vec3 forward = glm::normalize(glm::vec3(world[2]));
    glm::vec3 up = glm::normalize(glm::vec3(world[1]));

    return glm::lookAt(pos, pos - forward, up);
}

glm::mat4 CameraComponent::getProjectionMatrix() const {
	return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

bool CameraComponent::onAttach() {
    owner->getScene().onCameraAdded(this);
    return true;
}

void CameraComponent::onDetach() {
    if (owner->getScene().getActiveCamera() == this)
        owner->getScene().setActiveCamera(nullptr);
}

void CameraComponent::serialize(nlohmann::json& j) const {
    j["fov"] = fov;
    j["aspect"] = aspect;
    j["near"] = nearPlane;
    j["far"] = farPlane;
}

void CameraComponent::deserialize(const nlohmann::json& j) {
    fov = j.value("fov", 70.0f);
    aspect = j.value("aspect", 1.25f);
    nearPlane = j.value("near", 0.1f);
    farPlane = j.value("far", 100.0f);
}