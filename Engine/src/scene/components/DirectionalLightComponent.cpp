#include "DirectionalLightComponent.h"

#include <glm/glm.hpp>
#include <scene/Entity.h>
#include <persistance/ComponentFactory.h>

#include <scene/components/CameraComponent.h>

REGISTER(DirectionalLightComponent);

glm::mat4 DirectionalLightComponent::getLightSpaceMatrix(const glm::vec3& camPos) const {
    glm::vec3 dir = glm::normalize(getDirection());
    glm::vec3 lightPos = camPos - dir * 20.0f;

    glm::mat4 lightView = glm::lookAt(lightPos, camPos, glm::vec3(0, 1, 0));
    glm::mat4 lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);

    return lightProj * lightView;
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