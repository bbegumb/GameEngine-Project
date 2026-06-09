#include "MaterialComponent.h"

#include <core/AssetManager.h>
#include <persistance/ComponentFactory.h>
#include <renderer/Material.h>
#include <renderer/ShaderProgram.h>
#include <renderer/Texture.h>
#include <renderer/Mesh.h>
#include <renderer/ObjLoader.h>

#include "MeshComponent.h"

#include <glm/gtc/type_ptr.hpp>

REGISTER(MaterialComponent);

void MaterialComponent::serialize(nlohmann::json& j) const {
    j["materials"] = nlohmann::json::array();

    for (const auto& mat : materials) {
        if (!mat) continue;
        nlohmann::json mj;

        if (mat->shader)
            mj["shader"] = mat->shader->getName();

        const float* a = glm::value_ptr(mat->albedo);
        const float* e = glm::value_ptr(mat->emission);
        mj["albedo"] = std::vector<float>(a, a + 3);
        mj["emission"] = std::vector<float>(e, e + 3);
        mj["shininess"] = mat->shininess;
        mj["ambientReflectance"] = mat->ambientReflectance;
        mj["specularReflectance"] = mat->specularReflectance;
        mj["alpha"] = mat->alpha;
        mj["transparent"] = mat->transparent;

        if (mat->diffuseTexture)
            mj["diffuseTexture"] = mat->diffuseTexture->getName();

        j["materials"].push_back(mj);
    }
}

void MaterialComponent::deserialize(const nlohmann::json& j) {
    materials.clear();

    if (j.contains("materials") && !j["materials"].empty()) {
        for (const auto& mj : j["materials"]) {
            auto shader = AssetManager::getShader(mj.value("shader", "lit"));

            glm::vec3 albedo(0.5f);
            glm::vec3 emission(0.0f);
            if (mj.contains("albedo")) {
                auto a = mj["albedo"].get<std::vector<float>>();
                albedo = glm::make_vec3(a.data());
            }

            if (mj.contains("emission")) {
                auto e = mj["emission"].get<std::vector<float>>();
                emission = glm::make_vec3(e.data());
            }

            auto mat = std::make_shared<Material>(shader, albedo);
            mat->emission = emission;
            mat->shininess = mj.value("shininess", 32.0f);
            mat->ambientReflectance = mj.value("ambientReflectance", 0.5f);
            mat->specularReflectance = mj.value("specularReflectance", 0.5f);
            mat->alpha = mj.value("alpha", 1.0f);
            mat->transparent = mj.value("transparent", false);

            if (mj.contains("diffuseTexture")) {
                std::string texName = mj["diffuseTexture"];
                if (!texName.empty())
                    mat->diffuseTexture = AssetManager::getTexture(texName);
            }

            materials.push_back(mat);
        }
        return;
    }

    auto* mc = owner->getComponent<MeshComponent>();
    if (mc && mc->mesh && !mc->mesh->getName().empty()) {
        auto model = AssetManager::getModel(mc->mesh->getName());
        if (!model.materials.empty()) {
            materials = model.materials;
            return;
        }
    }

    auto shader = AssetManager::getShader("lit");
    materials.push_back(std::make_shared<Material>(shader));
}