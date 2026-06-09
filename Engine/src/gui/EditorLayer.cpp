#include "EditorLayer.h"

#include "imgui.h"

#include "ImGuizmo.h"
#include <core/Input.h>
#include <gui/Gizmohelp.h>
#include <controller/SceneController.h>
#include <controller/EntityController.h>
#include <controller/TransformController.h>
#include <controller/EditorSelectionController.h>
#include <scene/Scene.h>
#include <scene/Entity.h>
#include <scene/components/TransformComponent.h>
#include <scene/components/BehaviourComponent.h>
#include <scene/components/CameraComponent.h>
#include <scene/components/DirectionalLightComponent.h>
#include <scene/components/PointLightComponent.h>
#include <scene/components/MaterialComponent.h>
#include <scene/components/MeshComponent.h>
#include <scene/components/RigidBodyComponent.h>
#include <persistance/ComponentFactory.h>
#include <renderer/Texture.h>
#include <renderer/Material.h>
#include <renderer/Mesh.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <filesystem>

void EditorLayer::SetScene(Scene* scene, std::function<void()> onSave,
    std::function<void()> onLoad, std::function<void()> onExit,
    std::function<void()> onPlay, std::function<void()> onStop) {
    SceneController::setScene(scene);
    this->onSave = onSave;
    this->onLoad = onLoad;
    this->onExit = onExit;
    this->onPlay = onPlay;
    this->onStop = onStop;
}

void EditorLayer::initFileManager(const std::string& rootAssetsPath, const std::string& scriptsPath) {
    fileManager.init(rootAssetsPath);
    fileManager.initScriptsPath(scriptsPath);

    fileManager.onOpenScene = [this](const std::string& path) {
        if (onLoad) onLoad();
        };

    fileManager.onDropMesh = [](const std::string& path) {
        AssetManager::getMesh(std::filesystem::path(path).filename().string());
        };

    fileManager.onDropTexture = [](const std::string& path) {
        AssetManager::getTexture(std::filesystem::path(path).filename().string());
        };
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

    if (showScene)
        ShowScenePanel();

    if (showGame)
        ShowGamePanel();

    if (showFileManager)
        fileManager.show(&showFileManager);
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
            if (ImGui::MenuItem("New Scene")) {
                SceneController::getScene()->clear();
                EntityController::setSelectedEntity(nullptr);
            }
            if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
                if (onLoad) onLoad();
            }
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                if (onSave) onSave();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                if (onExit) onExit();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Stats", nullptr, &showStats);
            ImGui::MenuItem("Hierarchy", nullptr, &showHierarchy);
            ImGui::MenuItem("Inspector", nullptr, &showInspector);
            ImGui::MenuItem("Console", nullptr, &showConsole);
            ImGui::MenuItem("Scene", nullptr, &showScene);
            ImGui::MenuItem("Game", nullptr, &showGame);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        Scene* scene = SceneController::getScene();
        if (scene) {
            if (scene->isPlaying) {
                if (ImGui::MenuItem("Stop", "F5")) {
                    if (onStop) onStop();
                }
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
                ImGui::Text("PLAYING");
                ImGui::PopStyleColor();
            }
            else {
                if (ImGui::MenuItem("Play", "F5")) {
                    if (onPlay) onPlay();
                }
            }
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

    Entity* selectedEntity = GetValidSelectedEntity();

    if (!selectedEntity) {
        ImGui::Text("No entity selected.");
        ImGui::End();
        return;
    }

    ImGui::Text("Selected Entity: %s", selectedEntity->getName().c_str());
    ImGui::Separator();

    auto& transform = selectedEntity->transform;

    glm::vec3 position = transform.getPosition();
    glm::vec3 rotation = transform.getRotation();
    glm::vec3 scale = transform.getScale();

    float pos[3] = { position.x, position.y, position.z };
    float rot[3] = { rotation.x, rotation.y, rotation.z };
    float prev[3] = { rot[0], rot[1], rot[2] };
    float scl[3] = { scale.x, scale.y, scale.z };

    if (ImGui::DragFloat3("Position", pos, 0.1f)) {
        transform.setPosition({ pos[0], pos[1], pos[2] });
    }

    if (ImGui::DragFloat3("Rotation", rot, 0.01f)) {
        float dx = rot[0] - prev[0];
        float dy = rot[1] - prev[1];
        float dz = rot[2] - prev[2];

        if (dx != 0) transform.rotateAroundAxis(glm::vec3(1, 0, 0), dx);
        if (dy != 0) transform.rotateAroundAxis(glm::vec3(0, 1, 0), dy);
        if (dz != 0) transform.rotateAroundAxis(glm::vec3(0, 0, 1), dz);
    }

    if (ImGui::DragFloat3("Scale", scl, 0.1f, 0.1f, 100.0f)) {
        transform.setScale({ scl[0], scl[1], scl[2] });
    }

    if (auto* cam = selectedEntity->getComponent<CameraComponent>()) {
        if (ImGui::CollapsingHeader("Camera")) {
            ImGui::DragFloat("FOV", &cam->fov, 0.5f, 1.0f, 179.0f);
            ImGui::DragFloat("Near", &cam->nearPlane, 0.01f, 0.001f, 10.0f);
            ImGui::DragFloat("Far", &cam->farPlane, 1.0f, 1.0f, 10000.0f);
        }
    }

    if (auto* dl = selectedEntity->getComponent<DirectionalLightComponent>()) {
        if (ImGui::CollapsingHeader("Directional Light")) {
            ImGui::ColorEdit3("Color", &dl->color.x);
            ImGui::DragFloat("Ambient", &dl->ambientStrength, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Diffuse", &dl->diffuseStrength, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Specular", &dl->specularStrength, 0.01f, 0.0f, 5.0f);
        }
    }

    if (auto* pl = selectedEntity->getComponent<PointLightComponent>()) {
        if (ImGui::CollapsingHeader("Point Light")) {
            ImGui::ColorEdit3("Color", &pl->color.x);
            ImGui::DragFloat("Ambient", &pl->ambientStrength, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Diffuse", &pl->diffuseStrength, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Specular", &pl->specularStrength, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Constant", &pl->constant, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Linear", &pl->linear, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Quadratic", &pl->quadratic, 0.001f, 0.0f, 1.0f);
        }
    }

    if (auto* mc = selectedEntity->getComponent<MaterialComponent>()) {
        if (ImGui::CollapsingHeader("Material")) {
            int i = 0;
            while (mc->getMaterial(i)) {
                if (mc->getMaterial(i)) {
                    auto texture = mc->getMaterial(i)->diffuseTexture;
                    if (texture) ImGui::Text(texture->getName().c_str());
                    auto* mat = mc->getMaterial(i);

                    std::string numLabel = std::to_string(i + 1);
                    std::string texName = texture ? texture->getName() : "None  (drop image here)";
                    ImGui::Text("Texture %d: %s", i + 1, texName.c_str());

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* pl =
                            ImGui::AcceptDragDropPayload(FileManagerPanel::payload)) {
                            std::string path(static_cast<const char*>(pl->Data), pl->DataSize - 1);
                            std::string filename = std::filesystem::path(path).filename().string();
                            auto loadedTex = AssetManager::getTexture(filename);
                            if (loadedTex)
                                mat->diffuseTexture = loadedTex;
                        }
                        ImGui::EndDragDropTarget();
                    }

                    ImGui::ColorEdit3(("Albedo ##" + numLabel).c_str(), & mc->getMaterial(i)->albedo.x);
                    ImGui::DragFloat(("Shininess ##" + numLabel).c_str(), &mc->getMaterial(i)->shininess, 1.0f, 1.0f, 512.0f);
                    ImGui::DragFloat(("Ambient Ref ##" + numLabel).c_str(), &mc->getMaterial(i)->ambientReflectance, 0.01f, 0.0f, 1.0f);
                    ImGui::DragFloat(("Specular Ref ##" + numLabel).c_str(), &mc->getMaterial(i)->specularReflectance, 0.01f, 0.0f, 1.0f);
                }
                i++;
            }
        }
    }

    if (auto* mesh = selectedEntity->getComponent<MeshComponent>()) {
        if (ImGui::CollapsingHeader("Mesh")) {
            std::string name = mesh->mesh ? mesh->mesh->getName() : "None";
            ImGui::Text("Mesh: %s", name.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("(drop .obj/.fbx/.gltf here)");

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* pl =
                    ImGui::AcceptDragDropPayload(FileManagerPanel::payload)) {
                    std::string path(static_cast<const char*>(pl->Data), pl->DataSize - 1);
                    std::string filename = std::filesystem::path(path).filename().string();
                    auto loadedMesh = AssetManager::getMesh(filename);
                    if (loadedMesh)
                        mesh->mesh = loadedMesh;
                }
                ImGui::EndDragDropTarget();
            }
        }
    }

    if (auto* rb = selectedEntity->getComponent<RigidBodyComponent>()) {
        if (ImGui::CollapsingHeader("RigidBody")) {
            const char* types[] = { "Static", "Dynamic", "Kinematic" };
            int current = rb->getType();
            if (ImGui::Combo("Type", &current, types, 3)) {
                rb->setType(static_cast<RigidBodyType>(current));
            }
        }
    }

    for (auto& component : selectedEntity->getComponents()) {
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
    ImGui::Separator();
    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponentPopup");

    if (ImGui::BeginPopup("AddComponentPopup")) {
        if (!selectedEntity->getComponent<MeshComponent>()) {
            if (ImGui::MenuItem("Mesh"))
                selectedEntity->addComponent<MeshComponent>();
        }
        if (!selectedEntity->getComponent<MaterialComponent>()) {
            if (ImGui::MenuItem("Material"))
                selectedEntity->addComponent<MaterialComponent>();
        }
        if (!selectedEntity->getComponent<CameraComponent>()) {
            if (ImGui::MenuItem("Camera"))
                selectedEntity->addComponent<CameraComponent>();
        }
        if (!selectedEntity->getComponent<DirectionalLightComponent>()) {
            if (ImGui::MenuItem("Directional Light"))
                selectedEntity->addComponent<DirectionalLightComponent>();
        }
        if (!selectedEntity->getComponent<PointLightComponent>()) {
            if (ImGui::MenuItem("Point Light"))
                selectedEntity->addComponent<PointLightComponent>();
        }
        if (!selectedEntity->getComponent<RigidBodyComponent>()) {
            if (ImGui::MenuItem("RigidBody"))
                selectedEntity->addComponent<RigidBodyComponent>();
        }
        ImGui::Separator();

        auto scriptNames = fileManager.getScriptNames();
        if (scriptNames.empty()) {
            ImGui::TextDisabled("No scripts found");
            ImGui::TextDisabled("(add scripts via File Manager > Scripts tab)");
        }
        else {
            if (ImGui::BeginMenu("Script")) {
                for (auto& scriptName : scriptNames) {
					scriptName = scriptName.substr(0, scriptName.find_last_of('.'));
					if (ImGui::MenuItem(scriptName.c_str())) {
						ComponentFactory::create(scriptName, *selectedEntity, nlohmann::json());
					}
                }
                ImGui::EndMenu();
            }
        }

        ImGui::EndPopup();
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

void EditorLayer::ShowScenePanel() {
    static bool gizmoLocal = true;

    ImGui::Begin("Scene", &showScene);

    // Buttons first
    if (ImGui::Button("Translate")) Gizmo::setMode(GizmoMode::Translate);
    ImGui::SameLine();
    if (ImGui::Button("Rotate")) Gizmo::setMode(GizmoMode::Rotate);
    ImGui::SameLine();
    if (ImGui::Button("Scale")) Gizmo::setMode(GizmoMode::Scale);
    ImGui::SameLine();
    if (ImGui::Button(gizmoLocal ? "Local" : "World")) gizmoLocal = !gizmoLocal;

    if (ImGui::IsWindowFocused() && !Input::isMouseDown(MouseButton::Right)) {
        if (ImGui::IsKeyPressed(ImGuiKey_W)) Gizmo::setMode(GizmoMode::Translate);
        if (ImGui::IsKeyPressed(ImGuiKey_E)) Gizmo::setMode(GizmoMode::Rotate);
        if (ImGui::IsKeyPressed(ImGuiKey_R)) Gizmo::setMode(GizmoMode::Scale);
    }

    // Image ONCE, after buttons
    ImVec2 imagePos = ImGui::GetCursorScreenPos();
    ImVec2 imageSize = ImGui::GetContentRegionAvail();
    sceneViewSize = imageSize;

    if (sceneViewTexture && imageSize.x > 0 && imageSize.y > 0) {
        ImGui::Image((ImTextureID)(intptr_t)sceneViewTexture, imageSize,
            ImVec2(0, 1), ImVec2(1, 0));

        // Selection
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver()) {
            ImVec2 mouse = ImGui::GetMousePos();

            auto& picker = EditorSelectionController::getInstance().getColourPicker();
            picker.resize((int)imageSize.x, (int)imageSize.y);
            float aspect = imageSize.x / imageSize.y;
            glm::mat4 view = editorCamera.getViewMatrix();
            glm::mat4 proj = editorCamera.getProjectionMatrix(aspect);
            Scene* scene = SceneController::getScene();

            picker.renderPickingPass(scene, view, proj,
                [&](unsigned int pickShader, Entity* e, const glm::mat4& mvp) {
                    glUseProgram(pickShader);
                    glUniformMatrix4fv(glGetUniformLocation(pickShader, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
                    glm::vec3 col = picker.getPickedColour(e);
                    glUniform3fv(glGetUniformLocation(pickShader, "uColour"), 1, glm::value_ptr(col));
                    auto* mc = e->getComponent<MeshComponent>();
                    if (mc && mc->mesh) mc->mesh->draw();
                });

            EditorSelectionController::selectEntityFromViewport(
                mouse.x - imagePos.x, mouse.y - imagePos.y,
                imageSize.x, imageSize.y);
        }

        // ImGuizmo — use editor camera, not scene camera
        Entity* selected = EntityController::getSelectedEntity();
        if (selected) {
            glm::mat4 view = editorCamera.getViewMatrix();
            float aspect = imageSize.x / imageSize.y;
            glm::mat4 proj = editorCamera.getProjectionMatrix(aspect);
            glm::mat4 model = selected->transform.getMatrix();

            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

            ImGuizmo::OPERATION op;
            switch (Gizmo::getMode()) {
            case GizmoMode::Translate: op = ImGuizmo::TRANSLATE; break;
            case GizmoMode::Rotate:    op = ImGuizmo::ROTATE; break;
            case GizmoMode::Scale:     op = ImGuizmo::SCALE; break;
            default:                   op = ImGuizmo::TRANSLATE; break;
            }

            if (ImGuizmo::Manipulate(
                glm::value_ptr(view), glm::value_ptr(proj),
                op, gizmoLocal ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
                glm::value_ptr(model))) {

                glm::vec3 pos, rot, scl;
                ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(model),
                    glm::value_ptr(pos), glm::value_ptr(rot), glm::value_ptr(scl));

                selected->transform.setPosition(pos);
                selected->transform.setRotation(glm::radians(rot));
                selected->transform.setScale(scl);
            }
        }
    }

    ImGui::End();
}

void EditorLayer::ShowGamePanel() {
    ImGui::Begin("Game", &showGame);
    gameViewSize = ImGui::GetContentRegionAvail();
    if (gameViewTexture && gameViewSize.x > 0 && gameViewSize.y > 0) {
        ImGui::Image((ImTextureID)(intptr_t)gameViewTexture, gameViewSize,
            ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();
}

void EditorLayer::OnEntityRemoved(Entity* entity) {
    EntityController::setSelectedEntity(nullptr);
}

Entity* EditorLayer::GetValidSelectedEntity() {
    Entity* selectedEntity = EntityController::getSelectedEntity();
    Scene* scene = SceneController::getScene();
    if (!scene || !selectedEntity)
        return nullptr;

    for (const auto& entity : scene->getEntities()) {
        if (entity.get() == selectedEntity)
            return selectedEntity;
    }

    return nullptr;
}