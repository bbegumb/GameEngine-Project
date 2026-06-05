#pragma once

#include <imgui.h>
#include <gui/Gizmohelp.h>
#include <functional>
#include "EditorCamera.h"

class Scene;
class Entity;

class EditorLayer {
public:
    void SetScene(Scene* scene, std::function<void()> onSave,
        std::function<void()> onLoad, std::function<void()> onExit,
        std::function<void()> onPlay, std::function<void()> onStop);

    void OnUIRender();
    ImVec2 getSceneViewSize() const { return sceneViewSize; }
    ImVec2 getGameViewSize() const { return gameViewSize; }

    void OnEntityRemoved(Entity* entity);

    Entity* GetValidSelectedEntity();
    void OnUpdate(const glm::mat4& view, const glm::mat4& projection);

    EditorCamera editorCamera;
    unsigned int sceneViewTexture = 0;
    unsigned int gameViewTexture = 0;

    std::function<void()> onSave;
    std::function<void()> onLoad;
    std::function<void()> onExit;
    std::function<void()> onPlay;
    std::function<void()> onStop;

private:
    void ShowDockspace();
    void ShowMenuBar();
    void ShowStatsPanel();
    void ShowHierarchyPanel();
    void ShowInspectorPanel();
    void ShowConsolePanel();
    void ShowScenePanel();
    void ShowGamePanel();

private:
    ImVec2 sceneViewSize = ImVec2(1000, 800);
    ImVec2 gameViewSize = ImVec2(1000, 800);

    bool showStats = true;
    bool showHierarchy = true;
    bool showInspector = true;
    bool showConsole = true;
    bool showViewport = true;
    Gizmo gizmo;

    glm::mat4 cachedView = glm::mat4(1.0f);
    glm::mat4 cachedProjection = glm::mat4(1.0f);

    bool gizmoConsumedClick = false;

};
