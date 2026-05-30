#include "MeshComponent.h"

#include <core/AssetManager.h>
#include <persistance/ComponentFactory.h>
#include <renderer/Mesh.h>

REGISTER(MeshComponent);

void MeshComponent::serialize(nlohmann::json& j) const {
    j["mesh"] = mesh ? mesh->getName() : "";
}

void MeshComponent::deserialize(const nlohmann::json& j) {
    std::string name = j.value("mesh", "");
    if (!name.empty())
        mesh = AssetManager::getMesh(name);
}