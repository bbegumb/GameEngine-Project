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

    Entity& createEntity(const std::string& name, Entity* parent = nullptr);
    Entity& createEntityImmediate(const std::string& name, Entity* parent = nullptr);
    void removeEntity(Entity* entity);
    void removeEntity(const std::string& name);
    Entity* findEntity(const std::string& name);

    CameraComponent* getActiveCamera() const { return activeCamera; }
    void setActiveCamera(CameraComponent* camera) { activeCamera = camera; }
    void onCameraAdded(CameraComponent* camera);

    void onUpdate(float dt);
    void clear();

    const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }

    PhysicsWorld& getPhysicsWorld() { return physicsWorld; }

    std::function<void(Entity*)> onEntityRemoved;
    bool isPlaying = false;
private:
    void addNewEntities();
    void removeDeadEntities();

    float physicsAccumulator = 0.0f;
    float colliderCacheTimer = 0.0f;

    PhysicsWorld physicsWorld;

    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> entitiesToAdd;
    std::vector<Entity*> entitiesToRemove;
    CameraComponent* activeCamera = nullptr;
};