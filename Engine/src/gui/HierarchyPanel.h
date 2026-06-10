#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

class Entity;
class Scene;

class HierarchyPanel {
public:
    void show(Scene* scene, bool* open = nullptr);

    std::function<void(const std::string&)> onCreateEntity;
    std::function<void(Entity*)> onEntitySelected;
    std::function<Entity* ()> getSelectedEntity;
    std::function<void(Entity*)> onDeleteEntity;
    std::function<void(Entity*, const std::string&)> onRenameEntity;

private:
    void drawNode(Entity* e, const std::vector<std::unique_ptr<Entity>>& allEntities);

    static const char* DND_ID;
    Entity* renamingEntity = nullptr;
    char renameBuffer[256] = {};
};