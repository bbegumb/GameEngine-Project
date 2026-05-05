
#include "EntityController.h"

Entity* EntityController::selectedEntity = nullptr;

void EntityController::setSelectedEntity(Entity* entity) {
    selectedEntity = entity;
}

Entity* EntityController::getSelectedEntity() {
    return selectedEntity;
}

void EntityController::clearSelectedEntity() {
    selectedEntity = nullptr;
}

TransformComponent* EntityController::getTransform(Entity* entity) {
    if (!entity) return nullptr;
    return &entity->getTransform();
}

std::string EntityController::getName(Entity* entity) {
    if (!entity) return "";
    return entity->getName();
}

CameraComponent* EntityController::getCamera(Entity* entity){
    if(!entity || !entity->hasComponent<CameraComponent>()) return nullptr;
    return entity->getComponent<CameraComponent>();
}

MeshComponent* EntityController::getMesh(Entity* entity){
    if(!entity || !entity->hasComponent<MeshComponent>()) return nullptr;
    return entity->getComponent<MeshComponent>();
}

MaterialComponent* EntityController::getMaterial(Entity* entity){
    if(!entity || !entity->hasComponent<MaterialComponent>()) return nullptr;
    return entity->getComponent<MaterialComponent>();
}

PointLightComponent* EntityController::getPointLight(Entity* entity){
    if(!entity || !entity->hasComponent<PointLightComponent>()) return nullptr;
    return entity->getComponent<PointLightComponent>();
}

DirectionalLightComponent* EntityController::getDirectionalLight(Entity* entity){
    if(!entity || !entity->hasComponent<DirectionalLightComponent>()) return nullptr;
    return entity->getComponent<DirectionalLightComponent>();
}

void EntityController::addCamera(Entity* entity){
    if(!entity) return;
    if(!entity->hasComponent<CameraComponent>()){
        entity->addComponent<CameraComponent>(45.0f, 1.0f, 0.1f, 100.0f);
    }
}

void addMesh(Entity* entity){
    if(!entity) return;
    if(!entity->hasComponent<MeshComponent>()){
        entity->addComponent<MeshComponent>();
    }
}

void addMaterial(Entity* entity){
    if(!entity) return;
    if(!entity->hasComponent<MaterialComponent>()){
        entity->addComponent<MaterialComponent>();
    }
}

void addPointLight(Entity* entity){
    if(!entity) return;
    if(!entity->hasComponent<PointLightComponent>()){
        entity->addComponent<PointLightComponent>();
    }
}

void addDirectionalLight(Entity* entity){
    if(!entity) return;
    if(!entity->hasComponent<DirectionalLightComponent>()){
        entity->addComponent<DirectionalLightComponent>();
    }
}





