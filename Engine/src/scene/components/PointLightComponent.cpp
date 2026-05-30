#include "PointLightComponent.h"

#include <persistance/ComponentFactory.h>

REGISTER(PointLightComponent);

void PointLightComponent::serialize(nlohmann::json& j) const {
    const float* col = glm::value_ptr(color);
    j["color"] = std::vector<float>(col, col + 3);
    j["ambientStrength"] = ambientStrength;
    j["diffuseStrength"] = diffuseStrength;
    j["specularStrength"] = specularStrength;

    j["constant"] = constant;
    j["linear"] = linear;
    j["quadratic"] = quadratic;
}

void PointLightComponent::deserialize(const nlohmann::json& j) {
    auto col = j["color"].get<std::vector<float>>();
    color = glm::vec3(col[0], col[1], col[2]);
    ambientStrength = j.value("ambientStrength", 0.1f);
    diffuseStrength = j.value("diffuseStrength", 1.0f);
    specularStrength = j.value("specularStrength", 0.8f);

    constant = j.value("constant", 1.0f);
    linear = j.value("linear", 0.09f);
    quadratic = j.value("quadratic", 0.032f);
}