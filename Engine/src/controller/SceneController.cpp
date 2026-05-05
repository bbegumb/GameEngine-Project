#include "SceneController.h"

Scene* SceneController::currentScene = nullptr;

void SceneController::setScene(Scene* scene){
    currentScene = scene;
}

Scene* SceneController::getScene(){
    return currentScene;
}

Entity* SceneController::createEntity(const std::string& name){
    if(!currentScene) return nullptr;
    
    Entity& ref = currentScene->createEntity(name);
    return &ref; 
}

void SceneController::deleteEntity(Entity* entity){
    if(!currentScene || !entity) return;
    
    currentScene->removeEntity(entity);
}

const std::vector<std::unique_ptr<Entity>>& SceneController::getEntities() {
    return currentScene->getEntities();
}

std::vector<Entity*> SceneController::getEntityPointers() {
    std::vector<Entity*> result;
    
    for (const auto& e : currentScene->getEntities()) {
        result.push_back(e.get());
    }

    return result;
}
