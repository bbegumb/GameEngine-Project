#include <scene/components/MaterialComponent.h>

#include <core/AssetManager.h>
#include <persistance/ComponentFactory.h>
#include <persistance/Archive.h>

#include <renderer/Material.h>
#include <renderer/ShaderProgram.h>
#include <renderer/Texture.h>
#include <renderer/Mesh.h>
#include <renderer/ObjLoader.h>

#include <scene/components/MeshComponent.h>

REGISTER(MaterialComponent);

MaterialComponent::MaterialComponent(std::shared_ptr<Material> material) {
    if (material) materials.push_back(material);
    else {
        auto shader = AssetManager::getShader("lit");
        auto mat = std::make_shared<Material>(shader);
        materials.push_back(std::move(mat));
    }
}

Material* MaterialComponent::getMaterial(int index) const {
    if (index < 0 || index >= static_cast<int>(materials.size()))
        return nullptr;
    return materials[index].get();
}

void MaterialComponent::serialize(Archive& arch) const {
    for (const auto& mat : materials) {
        if (!mat) continue;
        Archive mj;

        if (mat->shader)
            mj.set("shader", mat->shader->getName());

        mj.set("albedo", mat->albedo);
        mj.set("emission", mat->emission);
        mj.set("shininess", mat->shininess);
        mj.set("ambientReflectance", mat->ambientReflectance);
        mj.set("specularReflectance", mat->specularReflectance);
        mj.set("alpha", mat->alpha);
        mj.set("transparent", mat->transparent);

        if (mat->diffuseTexture)
            mj.set("diffuseTexture", mat->diffuseTexture->getName());

        arch.append("materials", std::move(mj));
    }
}

void MaterialComponent::deserialize(const Archive& arch) {
    materials.clear();

    size_t count = arch.size("materials");
    if (count > 0) {
        for (size_t i = 0; i < count; i++) {
            Archive mj = arch.at("materials", i);

            std::string shaderName;
            if (!mj.get("shader", shaderName)) shaderName = "lit";
            auto shader = AssetManager::getShader(shaderName);

            glm::vec3 albedo(0.5f);
            glm::vec3 emission(0.0f);
            mj.get("albedo", albedo);
            mj.get("emission", emission);

            auto mat = std::make_shared<Material>(shader, albedo);
            mat->emission = emission;
            if (!mj.get("shininess", mat->shininess)) mat->shininess = 32.0f;
            if (!mj.get("ambientReflectance", mat->ambientReflectance)) mat->ambientReflectance = 0.5f;
            if (!mj.get("specularReflectance", mat->specularReflectance)) mat->specularReflectance = 0.5f;
            if (!mj.get("alpha", mat->alpha)) mat->alpha = 1.0f;
            if (!mj.get("transparent", mat->transparent)) mat->transparent = false;

            std::string texName;
            if (mj.get("diffuseTexture", texName) && !texName.empty())
                mat->diffuseTexture = AssetManager::getTexture(texName);

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

void MaterialComponent::addDefaultMaterial() {
    auto shader = AssetManager::getShader("lit");
    materials.push_back(std::make_shared<Material>(shader));
}