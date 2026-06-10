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

    hierarchyPanel.onCreateEntity = [](const std::string& name) {
        SceneController::createEntityImmediate(name);
        };
    hierarchyPanel.onEntitySelected = [](Entity* e) {
        EntityController::setSelectedEntity(e);
        };
    hierarchyPanel.getSelectedEntity = []() -> Entity* {
        return EntityController::getSelectedEntity();
        };
    hierarchyPanel.onRenameEntity = [](Entity* e, const std::string& newName) {
        e->setName(newName);
        };
    hierarchyPanel.onDeleteEntity = [](Entity* e) {
        SceneController::deleteEntity(e);
        EntityController::clearSelectedEntity();
        };
}

void EditorLayer::initFileManager(const std::string& rootAssetsPath, const std::string& scriptsPath) {
    fileManager.init(rootAssetsPath);
    fileManager.initScriptsPath(scriptsPath);

    fileManager.onOpenScene = [this](const std::string& sceneName) {
        SceneController::getScene()->newScene(sceneName);
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
    static bool openNewScene = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) 
                openNewScene = true;

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

    static bool focusName = false;

    if (openNewScene) {
        ImGui::OpenPopup("##new_scene");
        openNewScene = false;
        focusName = true;
    }

    if (ImGui::BeginPopupModal("##new_scene", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {

        static char sceneName[128] = "";
        ImGui::Text("Scene name");
        ImGui::Separator();
        ImGui::SetNextItemWidth(260.0f);
        if (focusName) {
            ImGui::SetKeyboardFocusHere();
            focusName = false;
        }
        bool confirm = ImGui::InputText("##name", sceneName, sizeof(sceneName),
            ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::Spacing();
        if (ImGui::Button("Create", ImVec2(126, 0)) || confirm) {
            if (strlen(sceneName) > 0) {
                EntityController::setSelectedEntity(nullptr);
                SceneController::newScene(sceneName);
                sceneName[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(126, 0))) {
            sceneName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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
    hierarchyPanel.show(SceneController::getScene(), &showHierarchy);
}

void EditorLayer::ShowInspectorPanel() {
    ImGui::Begin("Inspector", &showInspector);

    inspectorFocused = ImGui::IsWindowFocused();

    Entity* selectedEntity = GetValidSelectedEntity();

    if (!selectedEntity) {
        ImGui::Text("No entity selected.");
        ImGui::End();
        return;
    }

    ImGui::Text("Selected Entity: %s", selectedEntity->getName().c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("(drop script here)");
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* pl =
            ImGui::AcceptDragDropPayload(FileManagerPanel::scriptPayload)) {
            std::string path(static_cast<const char*>(pl->Data), pl->DataSize - 1);
            std::string scriptName = std::filesystem::path(path).stem().string();
            ComponentFactory::create(scriptName, *selectedEntity, nlohmann::json());
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::Separator();

    auto& transform = selectedEntity->transform;

    glm::vec3 position = transform.getPosition();
    glm::vec3 rotation = transform.getEulerRotation();
    glm::vec3 scale = transform.getScale();

    float pos[3] = { position.x, position.y, position.z };
    float rot[3] = { rotation.x, rotation.y, rotation.z };
    float prev[3] = { rot[0], rot[1], rot[2] };
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

    if (auto* cam = selectedEntity->getComponent<CameraComponent>()) {
        if (ImGui::CollapsingHeader("Camera")) {
            ImGui::DragFloat("FOV", &cam->fov, 0.5f, 1.0f, 179.0f);
            ImGui::DragFloat("Near", &cam->nearPlane, 0.01f, 0.001f, 10.0f);
            ImGui::DragFloat("Far", &cam->farPlane, 1.0f, 1.0f, 10000.0f);
        }
    }

    auto componentHeader = [](const char* label, bool* removeFlag,
        ImGuiTreeNodeFlags extraFlags = 0) -> bool {
            bool open = ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_AllowOverlap | extraFlags);
            ImGui::SameLine(ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX() - 20.0f);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 0.6f));
            std::string btnId = std::string("X##rm") + label;
            if (ImGui::SmallButton(btnId.c_str())) *removeFlag = true;
            ImGui::PopStyleColor(2);
            return open;
     };

    bool removeDL = false;
    if (auto* dl = selectedEntity->getComponent<DirectionalLightComponent>()) {
        if (componentHeader("Directional Light", &removeDL)) {
            ImGui::ColorEdit3("Color", &dl->color.x);
            ImGui::DragFloat("Ambient", &dl->ambientStrength, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Diffuse", &dl->diffuseStrength, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Specular", &dl->specularStrength, 0.01f, 0.0f, 5.0f);
        }
    }
    if (removeDL) selectedEntity->removeComponent<DirectionalLightComponent>();

    bool removePL = false;
    if (auto* pl = selectedEntity->getComponent<PointLightComponent>()) {
        if (componentHeader("Point Light", &removePL)) {
            ImGui::ColorEdit3("Color", &pl->color.x);
            ImGui::DragFloat("Ambient", &pl->ambientStrength, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Diffuse", &pl->diffuseStrength, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Specular", &pl->specularStrength, 0.01f, 0.0f, 5.0f);
            ImGui::DragFloat("Constant", &pl->constant, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Linear", &pl->linear, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Quadratic", &pl->quadratic, 0.001f, 0.0f, 1.0f);
        }
    }
    if (removePL) selectedEntity->removeComponent<PointLightComponent>();

    bool removeMat = false;
    if (auto* mc = selectedEntity->getComponent<MaterialComponent>()) {
        if (componentHeader("Material", &removeMat)) {
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

                    float alpha = mc->getMaterial(i)->alpha;
                    ImGui::ColorEdit3(("Albedo ##" + numLabel).c_str(), &mc->getMaterial(i)->albedo.x);
                    ImGui::ColorEdit3(("Emission ##" + numLabel).c_str(), &mc->getMaterial(i)->emission.x);
                    ImGui::DragFloat(("Shininess ##" + numLabel).c_str(), &mc->getMaterial(i)->shininess, 1.0f, 1.0f, 512.0f);
                    ImGui::DragFloat(("Ambient Ref ##" + numLabel).c_str(), &mc->getMaterial(i)->ambientReflectance, 0.01f, 0.01f, 1.0f);
                    ImGui::DragFloat(("Specular Ref ##" + numLabel).c_str(), &mc->getMaterial(i)->specularReflectance, 0.01f, 0.01f, 1.0f);
                    if (ImGui::DragFloat(("Alpha ##" + numLabel).c_str(), &alpha, 0.01f, 0.0f, 1.0f)) {
                        mc->getMaterial(i)->transparent = alpha < 1.0f;
                        mc->getMaterial(i)->alpha = alpha;
                    }
                }
                i++;
            }
            if (ImGui::Button("+ Add Material Slot")) {
                auto shader = AssetManager::getShader("lit");
                mc->materials.push_back(std::make_shared<Material>(shader));
            }
        }
    }
    if (removeMat) selectedEntity->removeComponent<MaterialComponent>();

    bool removeMesh = false;
    if (auto* mesh = selectedEntity->getComponent<MeshComponent>()) {
        if (componentHeader("Mesh", &removeMesh)) {
            std::string name = mesh->mesh ? mesh->mesh->getName() : "None";
            ImGui::Text("Mesh: %s", name.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("(drop .obj here)");

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload(FileManagerPanel::payload)) {
                    std::string path(static_cast<const char*>(payload->Data), payload->DataSize - 1);
                    std::string filename = std::filesystem::path(path).filename().string();
                    auto loadedModel = AssetManager::getModel(filename);
                    if (loadedModel.mesh)
                        mesh->mesh = loadedModel.mesh;
                    auto* matc = selectedEntity->getComponent<MaterialComponent>();
                    if (matc) matc->materials = loadedModel.materials;
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::SetNextItemWidth(-1);
            if (ImGui::BeginCombo("##primitives", "Set primitive...")) {
                for (auto& prim : { "cube", "sphere", "plane" }) {
                    if (ImGui::Selectable(prim))
                        mesh->mesh = AssetManager::getMesh(prim);
                }
                ImGui::EndCombo();
            }
        }
    }
    if (removeMesh) selectedEntity->removeComponent<MeshComponent>();

    bool removeRb = false;
    if (auto* rb = selectedEntity->getComponent<RigidBodyComponent>()) {
        if (componentHeader("RigidBody", &removeRb)) {
            const char* types[] = { "Static", "Dynamic", "Kinematic" };
            int current = rb->getType();
            if (ImGui::Combo("Type", &current, types, 3))
                rb->setType(static_cast<RigidBodyType>(current));
            }

            bool forceConvex = rb->getForceConvex();
            bool useTriangle = rb->getUseTriangleMesh();

            if (ImGui::Checkbox("Force Covex", &forceConvex)) {
                rb->setForceConvex(forceConvex);
            }

            if (ImGui::Checkbox("Use Triangle", &useTriangle)) {
                rb->setUseTriangleMesh(useTriangle);
            }

            ImGui::Separator();
            float sf = rb->getMaterial()->staticFriction;
            float df = rb->getMaterial()->dynamicFriction;
            float res = rb->getMaterial()->restitution;
            if (ImGui::DragFloat("Static Friction", &sf, 0.01f, 0.0f, 1.0f))
                rb->getMaterial()->setStaticFriction(sf);

            if (ImGui::DragFloat("Dynamic Friction", &df, 0.01f, 0.0f, 1.0f))
                rb->getMaterial()->setDynamicFriction(df);

            if (ImGui::DragFloat("Restitution", &res, 0.01f, 0.0f, 1.0f))
                rb->getMaterial()->setRestitution(res);
    }

    if (removeRb) selectedEntity->removeComponent<RigidBodyComponent>();

    BehaviourComponent* toRemove = nullptr;
    for (auto& component : selectedEntity->getComponents()) {
        auto* behaviour = dynamic_cast<BehaviourComponent*>(component.get());
        if (!behaviour) continue;

        ImGui::Separator();
        std::string name = typeid(*behaviour).name();
        if (name.find("class ") == 0) name = name.substr(6);

        bool removeBeh = false;
        if (componentHeader(name.c_str(), &removeBeh, ImGuiTreeNodeFlags_DefaultOpen)) {
            for (auto& prop : behaviour->getProperties()) {
                int compID = component->getID();
                std::string propName = prop.name + "##" + std::to_string(compID);
                switch (prop.type) {
                case PropertyType::Float:
                    ImGui::DragFloat(propName.c_str(), static_cast<float*>(prop.ptr), 0.1f);
                    break;
                case PropertyType::Double: {
                    float temp = static_cast<float>(*static_cast<double*>(prop.ptr));
                    if (ImGui::DragFloat(propName.c_str(), &temp, 0.1f))
                        *static_cast<double*>(prop.ptr) = static_cast<double>(temp);
                    break;
                }
                case PropertyType::Int:
                    ImGui::DragInt(propName.c_str(), static_cast<int*>(prop.ptr));
                    break;
                case PropertyType::Bool:
                    ImGui::Checkbox(propName.c_str(), static_cast<bool*>(prop.ptr));
                    break;
                case PropertyType::Vec2:
                    ImGui::DragFloat2(propName.c_str(), &static_cast<glm::vec2*>(prop.ptr)->x, 0.1f);
                    break;
                case PropertyType::Vec3:
                    ImGui::DragFloat3(propName.c_str(), &static_cast<glm::vec3*>(prop.ptr)->x, 0.1f);
                    break;
                case PropertyType::Vec4:
                    ImGui::DragFloat4(propName.c_str(), &static_cast<glm::vec4*>(prop.ptr)->x, 0.1f);
                    break;
                case PropertyType::Mat3: {
                    auto* m = static_cast<glm::mat3*>(prop.ptr);
                    ImGui::Text("%s", prop.name.c_str());
                    std::string id = "##" + propName;
                    ImGui::DragFloat3((id + "0").c_str(), &(*m)[0][0], 0.1f);
                    ImGui::DragFloat3((id + "1").c_str(), &(*m)[1][0], 0.1f);
                    ImGui::DragFloat3((id + "2").c_str(), &(*m)[2][0], 0.1f);
                    break;
                }
                case PropertyType::Mat4: {
                    auto* m = static_cast<glm::mat4*>(prop.ptr);
                    ImGui::Text("%s", prop.name.c_str());
                    std::string id = "##" + propName;
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
        if (removeBeh) { toRemove = behaviour; break; }
    }
    if (toRemove) selectedEntity->removeComponent<BehaviourComponent>();
    ImGui::Separator();
    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponentPopup");

    if (ImGui::BeginPopup("AddComponentPopup")) {
        if (!selectedEntity->getComponent<MeshComponent>()) {
            if (ImGui::MenuItem("Mesh")) {
                if (!selectedEntity->getComponent<MaterialComponent>())
                    selectedEntity->addComponent<MaterialComponent>();
                selectedEntity->addComponent<MeshComponent>();
            }
        }
        if (!selectedEntity->getComponent<MaterialComponent>()) {
            if (ImGui::MenuItem("Material")) {
                selectedEntity->addComponent<MaterialComponent>();
                if (auto* mc = selectedEntity->getComponent<MaterialComponent>())
                    mc->addDefaultMaterial();
            }
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
                    size_t dot = scriptName.find_last_of('.');
                    if (dot == std::string::npos) continue;
                    if (scriptName.substr(dot) != ".h") continue;

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

    ImVec2 imagePos = ImGui::GetCursorScreenPos();
    ImVec2 imageSize = ImGui::GetContentRegionAvail();
    sceneViewSize = imageSize;

    if (sceneViewTexture && imageSize.x > 0 && imageSize.y > 0) {
        ImGui::Image((ImTextureID)(intptr_t)sceneViewTexture, imageSize,
            ImVec2(0, 1), ImVec2(1, 0));

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

                glm::mat4 localMatrix = model;

                TransformComponent* parent = selected->transform.getParent();
                if (parent)
                    localMatrix = glm::inverse(parent->getMatrix()) * model;

                glm::vec3 pos, rot, scl;
                ImGuizmo::DecomposeMatrixToComponents(
                    glm::value_ptr(localMatrix),
                    glm::value_ptr(pos), glm::value_ptr(rot), glm::value_ptr(scl));

                selected->transform.setPosition(pos);
                selected->transform.setRotation(rot);
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