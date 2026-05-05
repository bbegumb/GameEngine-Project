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
    
    return currentScene->createEntity(name);
}

void SceneController::deleteEntity(Entity* entity){
    if(!currentScene || !entity) return nullptr;
    
    currentScene->removeEntity(entity);
}

const std::vector<std::unique_ptr<Entity>>& SceneController::getEntities() {
    return currentScene->getEntities();
}
