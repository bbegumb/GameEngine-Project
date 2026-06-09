#pragma once

#include <imgui.h>

#include <functional>
#include "EditorCamera.h"
#include <gui/FileManagerPanel.h>

class Scene;
class Entity;

class EditorLayer {
public:
    void SetScene(Scene* scene, std::function<void()> onSave,
        std::function<void()> onLoad, std::function<void()> onExit,
        std::function<void()> onPlay, std::function<void()> onStop);

    void initFileManager(const std::string& rootAssetsPath, const std::string& scriptsPath);

    void OnUIRender();
    ImVec2 getSceneViewSize() const { return sceneViewSize; }
    ImVec2 getGameViewSize() const { return gameViewSize; }

    void OnEntityRemoved(Entity* entity);

    Entity* GetValidSelectedEntity();

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
    ImVec2 sceneViewSize = ImVec2(1000, 800);
    ImVec2 gameViewSize = ImVec2(1000, 800);

    bool showStats = true;
    bool showHierarchy = true;
    bool showInspector = true;
    bool showConsole = true;
    bool showScene = true;
    bool showGame = true;
    bool showFileManager = true;

    FileManagerPanel fileManager;
};