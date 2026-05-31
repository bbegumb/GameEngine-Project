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
    ImVec2 GetViewportSize() const { return m_ViewportSize; }

    void OnEntityRemoved(Entity* entity);

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
    Entity* GetValidSelectedEntity();

    Scene* m_Scene = nullptr;
    Entity* m_SelectedEntity = nullptr;

    unsigned int m_ViewportTexture = 0;
    ImVec2 m_ViewportSize = ImVec2(0.0f, 0.0f);

    bool m_ShowStats = true;
    bool m_ShowHierarchy = true;
    bool m_ShowInspector = true;
    bool m_ShowConsole = true;
    bool m_ShowViewport = true;
};
