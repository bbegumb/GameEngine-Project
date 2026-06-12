#pragma once

#include <scene/Scene.h>
#include <scene/Entity.h>
#include <string>
#include <vector>
#include <memory>

class SceneController{
public:
    static void setScene(Scene* scene);
    static void newScene(const std::string& sceneName = "scene");
    static void openScene(const std::string& sceneName);
    static Scene* getScene();
    static Entity* createEntity(const std::string& name);
    static void deleteEntity(Entity* entity);
    static const std::vector<std::unique_ptr<Entity>>& getEntities();
    static std::vector<Entity*> getEntityPointers();
    static Entity* createEntityImmediate(const std::string& name);
private:
    static Scene* currentScene;
};

