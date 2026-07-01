#include <scene/components/MeshComponent.h>

#include <scene/Entity.h>
#include <core/AssetManager.h>

#include <persistance/ComponentFactory.h>
#include <persistance/Archive.h>

#include <renderer/Mesh.h>
#include <renderer/ObjLoader.h>

REGISTER(MeshComponent);

void MeshComponent::serialize(Archive& arch) const {
    if (!mesh) arch.set("mesh", "");
    else {
        if (mesh->getSubMeshCount() > 1) arch.set("model", mesh->getName());
        else arch.set("mesh", mesh->getName());
    }
}

void MeshComponent::deserialize(const Archive& arch) {
    std::string meshName;
    if (arch.get("mesh", meshName)) {
        mesh = AssetManager::getMesh(meshName);
    }
    else {
        std::string modelName;
        if (arch.get("model", modelName)) {
            auto model = AssetManager::getModel(modelName);
            mesh = model.mesh;
        }
    }
}