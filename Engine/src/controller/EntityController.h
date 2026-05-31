#pragma once

#include <string>

#include <scene/Entity.h>
#include <renderer/PrimitiveFactory.h>
#include <renderer/ShaderProgram.h>
#include <renderer/Material.h>
#include <scene/components/MeshComponent.h>
#include <scene/components/MaterialComponent.h>
#include <scene/components/PointLightComponent.h>
#include <scene/components/DirectionalLightComponent.h>
#include <scene/components/CameraComponent.h>
#include <scene/components/Component.h>


class EntityController{
public:
    static void setSelectedEntity(Entity* entity);
    static Entity* getSelectedEntity();
    static void clearSelectedEntity();
    static TransformComponent* getTransform(Entity* entity);
    static std::string getName(Entity* entity);
    static CameraComponent* getCamera(Entity* entity);
    static MeshComponent* getMesh(Entity* entity);
    static MaterialComponent* getMaterial(Entity* entity);
    static PointLightComponent* getPointLight(Entity* entity);
    static DirectionalLightComponent* getDirectionalLight(Entity* entity);
    static void addCamera(Entity* entity);
    static void addMesh(Entity* entity);
    static void addMaterial(Entity* entity);
    static void addPointLight(Entity* entity);
    static void addDirectionalLight(Entity* entity);

private:
    static Entity* selectedEntity;
};

