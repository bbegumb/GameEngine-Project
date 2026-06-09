#include "MeshComponent.h"

#include <scene/components/MaterialComponent.h>
#include <scene/Entity.h>
#include <core/AssetManager.h>
#include <persistance/ComponentFactory.h>
#include <renderer/Mesh.h>
#include <renderer/ObjLoader.h>

REGISTER(MeshComponent);

bool MeshComponent::onAttach() {
    auto* matc = owner->getComponent<MaterialComponent>();
    if (!matc) {
        owner->addComponent<MaterialComponent>();
    }

    return true;
}

void MeshComponent::serialize(nlohmann::json& j) const {
    if (!mesh) j["mesh"] = "";
    else {
        if (mesh->getSubMeshCount() > 1) j["model"] = mesh->getName();
        else j["mesh"] = mesh->getName();
    }
}

void MeshComponent::deserialize(const nlohmann::json& j) {

    if (j.contains("mesh")) {
        mesh = AssetManager::getMesh(j.value("mesh", ""));
    }
    else if (j.contains("model")) {
        auto model = AssetManager::getModel(j.value("model", ""));
        mesh = model.mesh;
    }
}