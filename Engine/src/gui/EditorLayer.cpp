#include "EditorLayer.h"

#include "imgui.h"

#include <controller/SceneController.h>
#include <controller/EntityController.h>
#include <controller/TransformController.h>

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
        if (ImGui::MenuItem("Camera")) {
            EntityController::addCamera(entity);
        }
        
        if (ImGui::MenuItem("Mesh")) {
            EntityController::addMesh(entity);
        }

        if (ImGui::MenuItem("Material")) {
            EntityController::addMaterial(entity);
        }

        if (ImGui::MenuItem("Point Light")) {
            EntityController::addPointLight(entity);
        }

        if (ImGui::MenuItem("Directional Light")) {
            EntityController::addDirectionalLight(entity);
        }
        ImGui::EndPopup();

    }
    
    ImGui::Separator();
    
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        glm::vec3 position = TransformController::getPosition(entity);
        glm::vec3 rotation = TransformController::getRotation(entity);
        glm::vec3 scale = TransformController::getScale(entity);

        if (ImGui::DragFloat3("Position", &position.x, 0.1f)) {
            TransformController::setPosition(entity, position);
        }

        if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f)) {
            TransformController::setRotation(entity, rotation);
        }

        if (ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.1f, 100.0f)) {
            TransformController::setScale(entity, scale);
        }
    }
    
    if (CameraComponent* camera = EntityController::getCamera(entity)) {
        if (ImGui::CollapsingHeader("Camera")) {
            ImGui::DragFloat("FOV", &camera->fov, 0.1f);
            ImGui::DragFloat("Aspect", &camera->aspect, 0.1f);
            ImGui::DragFloat("Near Plane", &camera->nearPlane, 0.01f);
            ImGui::DragFloat("Far Plane", &camera->farPlane, 1.0f);
        }
    }
    
    if (MeshComponent* mesh = EntityController::getMesh(entity)) {
        if (ImGui::CollapsingHeader("Mesh")) {
            ImGui::Text("Mesh Component exists");
            ImGui::Text("Mesh assigned: %s", mesh->mesh ? "Yes" : "No");
        }
    }

    if (MaterialComponent* material = EntityController::getMaterial(entity)) {
        if (ImGui::CollapsingHeader("Material")) {
            ImGui::Text("Material Component exists");
            ImGui::Text("Material assigned: %s", material->material ? "Yes" : "No");
        }
    }
    
    if (PointLightComponent* light = EntityController::getPointLight(entity)) {
        if (ImGui::CollapsingHeader("Point Light")) {
            ImGui::DragFloat3("Color", &light->color.x, 0.1f);
            ImGui::DragFloat("P_Ambient", &light->ambientStrength, 0.01f);
            ImGui::DragFloat("P_Diffuse", &light->diffuseStrength, 0.01f);
            ImGui::DragFloat("P_Specular", &light->specularStrength, 0.01f);
            ImGui::DragFloat("Constant", &light->constant, 0.01f);
            ImGui::DragFloat("Linear", &light->linear, 0.01f);
            ImGui::DragFloat("Quadratic", &light->quadratic, 0.01f);
        }
    }

    if (DirectionalLightComponent* light = EntityController::getDirectionalLight(entity)) {
        if (ImGui::CollapsingHeader("Directional Light")) {
            ImGui::DragFloat3("Direction", &light->direction.x, 0.1f);
            ImGui::DragFloat3("Color", &light->color.x, 0.1f);
            ImGui::DragFloat("Ambient", &light->ambientStrength, 0.01f);
            ImGui::DragFloat("Diffuse", &light->diffuseStrength, 0.01f);
            ImGui::DragFloat("Specular", &light->specularStrength, 0.01f);
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

     viewportSize = ImGui::GetContentRegionAvail();

    if (viewportTexture != 0 &&  viewportSize.x > 0.0f &&  viewportSize.y > 0.0f) {
        ImGui::Image(
            (ImTextureID)(intptr_t) viewportTexture,
             viewportSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
    } else {
        ImGui::Text("Viewport unavailable.");
    }

    ImGui::End();
}
