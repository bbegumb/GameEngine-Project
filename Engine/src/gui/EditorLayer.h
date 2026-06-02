#pragma once

#include <imgui.h>

#include <functional>

class Scene;
class Entity;

class EditorLayer {
public:
    void SetScene(Scene* scene);
    void SetViewportTexture(unsigned int textureID);

    void OnUIRender();
    ImVec2 GetViewportSize() const { return viewportSize; }

    void OnEntityRemoved(Entity* entity);

    Entity* GetValidSelectedEntity();

    std::function<void()> onSave;
    std::function<void()> onLoad;
private:
    void ShowDockspace();
    void ShowMenuBar();
    void ShowStatsPanel();
    void ShowHierarchyPanel();
    void ShowInspectorPanel();
    void ShowConsolePanel();
    void ShowViewportPanel();

private:
    unsigned int viewportTexture = 0;
    ImVec2 viewportSize = ImVec2(0.0f, 0.0f);

    bool showStats = true;
    bool showHierarchy = true;
    bool showInspector = true;
    bool showConsole = true;
    bool showViewport = true;
};
