#include "DirectionalLightComponent.h"

#include <glm/glm.hpp>
#include <scene/Entity.h>
#include <persistance/ComponentFactory.h>

#include <scene/components/CameraComponent.h>

REGISTER(DirectionalLightComponent);

glm::mat4 DirectionalLightComponent::getLightSpaceMatrix(const CameraComponent* camera) const {
    glm::vec3 camPos = camera->getEntity()->transform.getPosition();
    glm::vec3 camForward = camera->getEntity()->transform.forward();
    glm::vec3 center = camPos + camForward * shadowDistance * 0.5f;

    glm::vec3 lightDir = glm::normalize(getDirection());
    glm::vec3 lightPos = center - lightDir * shadowFar * 0.5f;

    glm::mat4 view = glm::lookAt(lightPos, center, owner->transform.up());
    glm::mat4 proj = glm::ortho(
        -shadowOrthoSize, shadowOrthoSize,
        -shadowOrthoSize, shadowOrthoSize,
        shadowNear, shadowFar
    );
    return proj * view;
}

glm::vec3 DirectionalLightComponent::getDirection() const {
    return owner->transform.forward();
}

void DirectionalLightComponent::serialize(nlohmann::json& j) const {
    const float* col = glm::value_ptr(color);
    j["color"] = std::vector<float>(col, col + 3);
    j["ambientStrength"] = ambientStrength;
    j["diffuseStrength"] = diffuseStrength;
    j["specularStrength"] = specularStrength;

    j["shadowDistance"] = shadowDistance;
    j["shadowOrthoSize"] = shadowOrthoSize;
    j["shadowNear"] = shadowNear;
    j["shadowFar"] = shadowFar;
}

void DirectionalLightComponent::deserialize(const nlohmann::json& j) {
    auto col = j["color"].get<std::vector<float>>();
    color = glm::vec3(col[0], col[1], col[2]);
    ambientStrength = j.value("ambientStrength", 0.2f);
    diffuseStrength = j.value("diffuseStrength", 1.0f);
    specularStrength = j.value("specularStrength", 0.5f);

    shadowDistance = j.value("shadowDistance", 30.0f);
    shadowOrthoSize = j.value("shadowOrthoSize", 40.0f);
    shadowNear = j.value("shadowNear", 0.1f);
    shadowFar = j.value("shadowFar", 200.0f);
}