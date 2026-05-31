#include "EditorLayer.h"

#include "imgui.h"
#include <gui/Gizmohelp.h>
#include <controller/SceneController.h>
#include <controller/EntityController.h>
#include <controller/TransformController.h>
#include <reflection/Reflection.h>
#include <reflection/ReflectionDraw.h>
#include <controller/EditorSelectionController.h>
#include <scene/Scene.h>
#include <scene/Entity.h>
#include <scene/components/CameraComponent.h>
#include <scene/components/TransformComponent.h>
#include <glm/glm.hpp>
#include <cmath>

void EditorLayer::SetScene(Scene* scene) {
    SceneController::setScene(scene);
}

void EditorLayer::SetViewportTexture(unsigned int textureID) {
     viewportTexture = textureID;
}

void EditorLayer::OnUIRender() {
    ShowDockspace();
    ShowMenuBar();

    if (showStats)
        ShowStatsPanel();

    if (showHierarchy)
        ShowHierarchyPanel();

    if (showInspector)
        ShowInspectorPanel();

    if (showConsole)
        ShowConsolePanel();

    if (showViewport)
        ShowViewportPanel();
}

void EditorLayer::ShowDockspace() {
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    windowFlags |= ImGuiWindowFlags_NoTitleBar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    windowFlags |= ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("MainDockSpace", nullptr, windowFlags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
}

void EditorLayer::ShowMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New Scene");
            ImGui::MenuItem("Open Scene");
            ImGui::MenuItem("Save Scene");
            ImGui::Separator();
            ImGui::MenuItem("Exit");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Stats", nullptr, &showStats);
            ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &showInspector);
            ImGui::MenuItem("Console", nullptr, &showConsole);
            ImGui::MenuItem("Viewport", nullptr, &showViewport);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void EditorLayer::ShowStatsPanel() {
    ImGui::Begin("Stats", &showStats);

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("FPS: %.1f", io.Framerate);

    if (io.Framerate > 0.0f)
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / io.Framerate);
    else
        ImGui::Text("Frame Time: N/A");

    ImGui::Separator();
    ImGui::Text("Renderer: OpenGL");
    ImGui::Text("Editor Mode: Active");

    ImGui::End();
}

void EditorLayer::ShowHierarchyPanel() {
    ImGui::Begin("Hierarchy", &showHierarchy);

    if (ImGui::Button("Add Entity")) {
            SceneController::createEntity("New Entity");
    }
    
    ImGui::Separator();
    
    for (Entity* entity : SceneController::getEntityPointers()) {
           if (!entity) continue;

           bool selected = (EntityController::getSelectedEntity() == entity);

           if (ImGui::Selectable(EntityController::getName(entity).c_str(), selected)) {
               EntityController::setSelectedEntity(entity);
       }
    }

    ImGui::End();
}

void EditorLayer::ShowInspectorPanel() {
    ImGui::Begin("Inspector", &showInspector);

    Entity* entity = EntityController::getSelectedEntity();
    
    if (!entity) {
            ImGui::Text("No entity selected.");
            ImGui::End();
            return;
    }

    ImGui::Text("Selected Entity: %s", entity->getName().c_str());
    ImGui::Separator();

    if(ImGui::Button("Add Component")){
        ImGui::OpenPopup("AddComponentPopup");
    }
    
    if (ImGui::BeginPopup("AddComponentPopup")) {
        for (auto& [typeName, typeInfo] : ReflectionRegistry::getTypes()) {
            if (typeName == "TransformComponent")
                continue;

            if (ImGui::MenuItem(typeInfo.name.c_str())) {
                if (typeInfo.addToEntity) {
                    typeInfo.addToEntity(entity);
                }
            }
        }

        ImGui::EndPopup();
    }

    ImGui::Separator();
    
    for (auto& componentPtr : entity->getComponents()) {
        Component* component = componentPtr.get();

        TypeInfo* type = ReflectionRegistry::getType(component);

        if (type && ImGui::CollapsingHeader(type->name.c_str())) {
            DrawReflectedFields(component, type);
        }
    }
    
    ImGui::End();

}

void EditorLayer::ShowConsolePanel() {
    ImGui::Begin("Console", &showConsole);

    ImGui::TextWrapped("[Info] Engine initialized successfully.");
    ImGui::TextWrapped("[Info] ImGui editor loaded.");

    if (SceneController::getScene())
        ImGui::TextWrapped("[Debug] Scene connected.");
    else
        ImGui::TextWrapped("[Debug] No scene connected.");

    ImGui::End();
}

void EditorLayer::ShowViewportPanel() {
    ImGui::Begin("Viewport", &showViewport);

    if (viewportTexture == 0) {
        ImGui::Text("Viewport unavailable.");
        ImGui::End();
        return;
    }

    if (ImGui::Button("Translate")) {
        Gizmo::setMode(GizmoMode::Translate);
    }

    ImGui::SameLine();

    if (ImGui::Button("Rotate")) {
        Gizmo::setMode(GizmoMode::Rotate);
    }

    ImGui::SameLine();

    if (ImGui::Button("Scale")) {
        Gizmo::setMode(GizmoMode::Scale);
    }

    ImGui::Separator();

    if (ImGui::IsWindowFocused()) {
        if (ImGui::IsKeyPressed(ImGuiKey_W))
            Gizmo::setMode(GizmoMode::Translate);

        if (ImGui::IsKeyPressed(ImGuiKey_E))
            Gizmo::setMode(GizmoMode::Rotate);

        if (ImGui::IsKeyPressed(ImGuiKey_R))
            Gizmo::setMode(GizmoMode::Scale);
    }

    ImVec2 imagePos = ImGui::GetCursorScreenPos();
    ImVec2 imageSize = ImGui::GetContentRegionAvail();

    viewportSize = imageSize;

    if (imageSize.x <= 0.0f || imageSize.y <= 0.0f) {
        ImGui::Text("Viewport size invalid.");
        ImGui::End();
        return;
    }

    ImGui::Image(
        (ImTextureID)(intptr_t)viewportTexture,
        imageSize,
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        ImVec2 mouse = ImGui::GetMousePos();

        float localX = mouse.x - imagePos.x;
        float localY = mouse.y - imagePos.y;

        EditorSelectionController::selectEntityFromViewport(
            localX,
            localY,
            imageSize.x,
            imageSize.y
        );
    }

    if (ImGui::IsItemHovered()) {
        float scroll = ImGui::GetIO().MouseWheel;
        if (scroll != 0.0f) {
            Scene* scene = SceneController::getScene();
            if (scene) {
                CameraComponent* cam = scene->getActiveCamera();
                if (cam) {
                    Entity* camEntity = cam->getEntity();
                    TransformComponent& t = camEntity->getTransform();

                    glm::vec3 rot = t.getRotation();
                    glm::vec3 forward;
                    forward.x = std::sin(rot.y) * std::cos(rot.x);
                    forward.y = -std::sin(rot.x);
                    forward.z = -std::cos(rot.y) * std::cos(rot.x);

                    const float zoomSpeed = 0.5f;
                    t.setPosition(t.getPosition() + forward * scroll * zoomSpeed);
                }
            }
        }
    }

    ImGui::End();
}
