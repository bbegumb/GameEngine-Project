#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

class Entity;
class CameraComponent;
class PhysicsWorld;

class Scene {
public:
    Scene(const std::string& sceneName = "scene");
    ~Scene();

    static void createDefaultScene(Scene& scene);

    void renameScene(const std::string& name) { sceneName = name; }
    void newScene(const std::string& sceneName = "scene");
    void openScene(const std::string& sceneName);

    std::string getSceneName() const { return sceneName; }
    void setSceneName(const std::string& name) { sceneName = name; }

    Entity& createEntity(const std::string& name, Entity* parent = nullptr);
    Entity& createEntityImmediate(const std::string& name, Entity* parent = nullptr);
    void removeEntity(Entity* entity);
    void removeEntity(const std::string& name);
    Entity* findEntity(const std::string& name);

    CameraComponent* getActiveCamera() const { return activeCamera; }
    void setActiveCamera(CameraComponent* camera);
    void onCameraAdded(CameraComponent* camera);

    void onUpdate(float dt);
    void clear();

    const std::vector<std::unique_ptr<Entity>>& getEntities() const { return entities; }

    PhysicsWorld& getPhysicsWorld();

    std::function<void(Entity*)> onEntityRemoved;
    bool isPlaying = false;
private:
    void addNewEntities();
    void removeDeadEntities();

    float physicsAccumulator = 0.0f;
    float colliderCacheTimer = 0.0f;

    int nextEntityID = 0;

    std::unique_ptr<PhysicsWorld> physicsWorld;

    std::string sceneName;

    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> entitiesToAdd;
    std::vector<Entity*> entitiesToRemove;
    CameraComponent* activeCamera = nullptr;
};
