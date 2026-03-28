#pragma once

#include <memory>
#include <string>
#include <vector>

class Entity;
class CameraComponent;

class Scene {
public:
    Entity& createEntity(const std::string& name);

    CameraComponent* getActiveCamera() const { return activeCamera; }
    void setActiveCamera(CameraComponent* camera) { activeCamera = camera; }
    void onCameraAdded(CameraComponent* camera);

    void onUpdate(float dt);

    const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }

private:
    std::vector<std::unique_ptr<Entity>> entities;
    CameraComponent* activeCamera = nullptr;
};