#include "EditorLayer.h"

#include "imgui.h"

#include <scene/Scene.h>
#include <scene/Entity.h>
#include <scene/components/TransformComponent.h>
#include <scene/components/BehaviourComponent.h>

void EditorLayer::SetScene(Scene* scene) {
    m_Scene = scene;
}

void EditorLayer::SetViewportTexture(unsigned int textureID) {
    m_ViewportTexture = textureID;
}

void EditorLayer::OnUIRender() {
    ShowDockspace();
    ShowMenuBar();

    if (m_ShowStats)
        ShowStatsPanel();

    if (m_ShowHierarchy)
        ShowHierarchyPanel();

    if (m_ShowInspector)
        ShowInspectorPanel();

    if (m_ShowConsole)
        ShowConsolePanel();

    if (m_ShowViewport)
        ShowViewportPanel();
}

void EditorLayer::OnEntityRemoved(Entity* entity) {
    if (m_SelectedEntity == entity)
        m_SelectedEntity = nullptr;
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
            ImGui::MenuItem("Stats", nullptr, &m_ShowStats);
            ImGui::MenuItem("Hierarchy", nullptr, &m_ShowHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &m_ShowInspector);
            ImGui::MenuItem("Console", nullptr, &m_ShowConsole);
            ImGui::MenuItem("Viewport", nullptr, &m_ShowViewport);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void EditorLayer::ShowStatsPanel() {
    ImGui::Begin("Stats", &m_ShowStats);

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
    ImGui::Begin("Hierarchy", &m_ShowHierarchy);

    if (!m_Scene) {
        ImGui::Text("No scene loaded.");
        ImGui::End();
        return;
    }

    for (const auto& entityPtr : m_Scene->getEntities()) {
        Entity* entity = entityPtr.get();
        bool selected = (m_SelectedEntity == entity);

        if (ImGui::Selectable(entity->getName().c_str(), selected)) {
            m_SelectedEntity = entity;
        }
    }

    ImGui::End();
}

void EditorLayer::ShowInspectorPanel() {
    ImGui::Begin("Inspector", &m_ShowInspector);

    if (!m_SelectedEntity) {
        ImGui::Text("No entity selected.");
        ImGui::End();
        return;
    }

    ImGui::Text("Selected Entity: %s", m_SelectedEntity->getName().c_str());
    ImGui::Separator();

    auto& transform = m_SelectedEntity->transform;

    glm::vec3 position = transform.getPosition();
    glm::vec3 rotation = transform.getRotation();
    glm::vec3 scale = transform.getScale();

    float pos[3] = { position.x, position.y, position.z };
    float rot[3] = { rotation.x, rotation.y, rotation.z };
    float scl[3] = { scale.x, scale.y, scale.z };

    if (ImGui::DragFloat3("Position", pos, 0.1f)) {
        transform.setPosition({ pos[0], pos[1], pos[2] });
    }

    if (ImGui::DragFloat3("Rotation", rot, 0.1f)) {
        transform.setRotation({ rot[0], rot[1], rot[2] });
    }

    if (ImGui::DragFloat3("Scale", scl, 0.1f, 0.1f, 100.0f)) {
        transform.setScale({ scl[0], scl[1], scl[2] });
    }

    for (auto& component : m_SelectedEntity->getComponents()) {
        auto* behaviour = dynamic_cast<BehaviourComponent*>(component.get());
        if (!behaviour) continue;

        ImGui::Separator();
        std::string name = typeid(*behaviour).name();
        if (name.find("class ") == 0)
            name = name.substr(6);
        if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            for (auto& prop : behaviour->getProperties()) {
                switch (prop.type) {
                case PropertyType::Float:
                    ImGui::DragFloat(prop.name.c_str(), static_cast<float*>(prop.ptr), 0.1f);
                    break;
                case PropertyType::Double: {
                    float temp = static_cast<float>(*static_cast<double*>(prop.ptr));
                    if (ImGui::DragFloat(prop.name.c_str(), &temp, 0.1f))
                        *static_cast<double*>(prop.ptr) = static_cast<double>(temp);
                    break;
                }
                case PropertyType::Int:
                    ImGui::DragInt(prop.name.c_str(), static_cast<int*>(prop.ptr));
                    break;
                case PropertyType::Bool:
                    ImGui::Checkbox(prop.name.c_str(), static_cast<bool*>(prop.ptr));
                    break;
                case PropertyType::Vec2:
                    ImGui::DragFloat2(prop.name.c_str(), &static_cast<glm::vec2*>(prop.ptr)->x, 0.1f);
                    break;
                case PropertyType::Vec3:
                    ImGui::DragFloat3(prop.name.c_str(), &static_cast<glm::vec3*>(prop.ptr)->x, 0.1f);
                    break;
                case PropertyType::Vec4:
                    ImGui::DragFloat4(prop.name.c_str(), &static_cast<glm::vec4*>(prop.ptr)->x, 0.1f);
                    break;
                case PropertyType::Mat3: {
                    auto* m = static_cast<glm::mat3*>(prop.ptr);
                    ImGui::Text("%s", prop.name.c_str());
                    std::string id = "##" + prop.name;
                    ImGui::DragFloat3((id + "0").c_str(), &(*m)[0][0], 0.1f);
                    ImGui::DragFloat3((id + "1").c_str(), &(*m)[1][0], 0.1f);
                    ImGui::DragFloat3((id + "2").c_str(), &(*m)[2][0], 0.1f);
                    break;
                }
                case PropertyType::Mat4: {
                    auto* m = static_cast<glm::mat4*>(prop.ptr);
                    ImGui::Text("%s", prop.name.c_str());
                    std::string id = "##" + prop.name;
                    ImGui::DragFloat4((id + "0").c_str(), &(*m)[0][0], 0.1f);
                    ImGui::DragFloat4((id + "1").c_str(), &(*m)[1][0], 0.1f);
                    ImGui::DragFloat4((id + "2").c_str(), &(*m)[2][0], 0.1f);
                    ImGui::DragFloat4((id + "3").c_str(), &(*m)[3][0], 0.1f);
                    break;
                }
                default:
                    break;
                }
            }
        }
    }

    ImGui::End();
}

void EditorLayer::ShowConsolePanel() {
    ImGui::Begin("Console", &m_ShowConsole);

    ImGui::TextWrapped("[Info] Engine initialized successfully.");
    ImGui::TextWrapped("[Info] ImGui editor loaded.");

    if (m_Scene)
        ImGui::TextWrapped("[Debug] Scene connected.");
    else
        ImGui::TextWrapped("[Debug] No scene connected.");

    ImGui::End();
}

void EditorLayer::ShowViewportPanel() {
    ImGui::Begin("Viewport", &m_ShowViewport);

    m_ViewportSize = ImGui::GetContentRegionAvail();

    if (m_ViewportTexture != 0 && m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f) {
        ImGui::Image(
            (ImTextureID)(intptr_t)m_ViewportTexture,
            m_ViewportSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
    } else {
        ImGui::Text("Viewport unavailable.");
    }

    ImGui::End();
}
