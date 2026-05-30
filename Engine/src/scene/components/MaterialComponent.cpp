#include "MaterialComponent.h"

#include <core/AssetManager.h>
#include <persistance/ComponentFactory.h>
#include <renderer/Material.h>
#include <renderer/ShaderProgram.h>

#include <glm/gtc/type_ptr.hpp>

REGISTER(MaterialComponent);

void MaterialComponent::serialize(nlohmann::json& j) const {
    if (!material) return;

    j["shader"] = material->shader ? material->shader->getName() : "lit";

    const float* a = glm::value_ptr(material->albedo);
    j["albedo"] = std::vector<float>(a, a + 3);
    j["shininess"] = material->shininess;
    j["ambientReflectance"] = material->ambientReflectance;
    j["specularReflectance"] = material->specularReflectance;
}

void MaterialComponent::deserialize(const nlohmann::json& j) {
    std::string shaderName = j.value("shader", "lit");
    auto shader = AssetManager::getShader(shaderName);

    auto albedo = j["albedo"].get<std::vector<float>>();
    material = std::make_shared<Material>(shader, glm::make_vec3(albedo.data()));

    material->shininess = j.value("shininess", 32.0f);
    material->ambientReflectance = j.value("ambientReflectance", 0.5f);
    material->specularReflectance = j.value("specularReflectance", 0.5f);
}