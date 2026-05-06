#pragma once

#include <memory>
#include <string>
#include <vector>
#include <physics/PhysicsWorld.h>

class Entity;
class CameraComponent;

class Scene {
public:
    Scene();
    ~Scene();

    Entity& createEntity(const std::string& name);

    CameraComponent* getActiveCamera() const { return activeCamera; }
    void setActiveCamera(CameraComponent* camera) { activeCamera = camera; }
    void onCameraAdded(CameraComponent* camera);

    void onUpdate(float dt);

    const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }

    PhysicsWorld getPhysicsWorld() const { return physicsWorld; }

private:
    float physicsAccumulator = 0.0f;

    PhysicsWorld physicsWorld;

    std::vector<std::unique_ptr<Entity>> entities;
    CameraComponent* activeCamera = nullptr;
};