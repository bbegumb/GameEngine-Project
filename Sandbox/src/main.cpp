#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

#include <imgui.h>
#include <ImGuizmo.h>

#include <controller/EntityController.h>
#include <persistance/SceneSerializer.h>
#include <core/Input.h>
#include <renderer/Renderer.h>

#include "DemoScene.h"

#include "gui/ImGuiLayer.h"
#include "gui/EditorLayer.h"
#include "gui/ViewportFramebuffer.h"
#include <scripts/KinematicMover.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1000, 800, "Dev Window", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 1000, 800);

    Input::init(window);

    Scene scene;
    Renderer renderer;
    
    std::string defaultScenePath = std::string(SCENES_PATH) + "scene.json";

    std::ifstream sceneCheck(defaultScenePath);
    if (sceneCheck.good()) {
        sceneCheck.close();
        SceneSerializer::load(scene, "scene.json");
        printf("Loaded scene from %s\n", defaultScenePath.c_str());
    }
    else {
        DemoScene::createScene2(scene);
        printf("No saved scene found, created default\n");
        scene.onUpdate(0.0f);
        SceneSerializer::save(scene, "scene.json");
    }

    ImGuiLayer imguiLayer;
    imguiLayer.Init(window);

    EditorLayer editorLayer;
    auto onSave = [&scene, &defaultScenePath]() {
        SceneSerializer::save(scene, "scene.json");
        printf("Scene saved!\n");
    };
    auto onLoad = [&scene, &editorLayer, &defaultScenePath]() {
        scene.clear();
        SceneSerializer::load(scene, "scene.json");
        editorLayer.OnEntityRemoved(nullptr);
        printf("Scene loaded!\n");
    };
    auto onExit = []() {
        glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
    };
    auto onPlay = [&scene]() {
        SceneSerializer::save(scene, "_temp.json");
        scene.isPlaying = true;
        };
    auto onStop = [&scene]() {
        scene.isPlaying = false;
        scene.clear();
        EntityController::setSelectedEntity(nullptr);
        SceneSerializer::load(scene, "_temp.json");    
    };

    editorLayer.SetScene(&scene, onSave, onLoad, onExit, onPlay, onStop);

    editorLayer.initFileManager((std::filesystem::current_path() / "assets").string(), (std::filesystem::current_path() / "scripts").string());

    ViewportFramebuffer gameFramebuffer;
    gameFramebuffer.Init(1000, 800);

    ViewportFramebuffer sceneFramebuffer;
    sceneFramebuffer.Init(1000, 800);

    scene.onEntityRemoved = [&editorLayer](Entity* entity) {
        editorLayer.OnEntityRemoved(entity);
    };

    float last = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float now = static_cast<float>(glfwGetTime());
        if (last == 0.0f) {
            last = now;
        }

        float dt = now - last;
        last = now;

        glfwPollEvents();
        
        Input::update();

        if (Input::isKeyDown(Key::LeftControl) && Input::isKeyPressed(Key::S) && !scene.isPlaying) {
            SceneSerializer::save(scene, "scene.json");
            printf("Scene saved!\n");
        }

        if (Input::isKeyDown(Key::LeftControl) && Input::isKeyPressed(Key::O)) {
            scene.clear();
            SceneSerializer::load(scene, "scene.json");
            printf("Scene loaded!\n");
        }

        editorLayer.editorCamera.update(dt);
        scene.onUpdate(dt);

        // Scene view — editor camera
        ImVec2 sceneSize = editorLayer.getSceneViewSize();
        unsigned int sw = static_cast<unsigned int>(sceneSize.x);
        unsigned int sh = static_cast<unsigned int>(sceneSize.y);

        if (sw > 0 && sh > 0) {
            sceneFramebuffer.Resize(sw, sh);
            float aspect = static_cast<float>(sw) / static_cast<float>(sh);
            sceneFramebuffer.Bind();
            renderer.render(scene,
                editorLayer.editorCamera.getViewMatrix(),
                editorLayer.editorCamera.getProjectionMatrix(aspect));
            sceneFramebuffer.Unbind();
            editorLayer.sceneViewTexture = sceneFramebuffer.GetColorAttachment();
        }

        // Game view — scene camera
        ImVec2 gameSize = editorLayer.getGameViewSize();
        unsigned int gw = static_cast<unsigned int>(gameSize.x);
        unsigned int gh = static_cast<unsigned int>(gameSize.y);

        if (gw > 0 && gh > 0 && scene.getActiveCamera()) {
            gameFramebuffer.Resize(gw, gh);
            scene.getActiveCamera()->aspect = static_cast<float>(gw) / static_cast<float>(gh);
            gameFramebuffer.Bind();
            renderer.render(scene);
            gameFramebuffer.Unbind();
            editorLayer.gameViewTexture = gameFramebuffer.GetColorAttachment();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        int windowWidth = 0;
        int windowHeight = 0;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        imguiLayer.Begin();
        ImGuizmo::BeginFrame();
        editorLayer.OnUIRender();
        imguiLayer.End();

        glfwSwapBuffers(window);
    }

    imguiLayer.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
