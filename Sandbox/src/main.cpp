#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

#include <persistance/SceneSerializer.h>
#include <core/Input.h>
#include <renderer/Renderer.h>
#include <scene/scene_all.h>
#include <DemoScene.h>

#include "gui/ImGuiLayer.h"
#include "gui/EditorLayer.h"
#include "gui/ViewportFramebuffer.h"

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
    editorLayer.SetScene(&scene);

    editorLayer.onSave = [&scene, &defaultScenePath]() {
        SceneSerializer::save(scene, "scene.json");
        printf("Scene saved!\n");
    };

    editorLayer.onLoad = [&scene, &editorLayer, &defaultScenePath]() {
        scene.clear();
        SceneSerializer::load(scene, "scene.json");
        editorLayer.OnEntityRemoved(nullptr);
        printf("Scene loaded!\n");
    };

    ViewportFramebuffer viewportFramebuffer;
    viewportFramebuffer.Init(1000, 800);

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

        if (Input::isKeyDown(Key::LeftControl) && Input::isKeyPressed(Key::S) && !scene.isPlaying) {
            SceneSerializer::save(scene, "scene.json");
            printf("Scene saved!\n");
        }

        if (Input::isKeyDown(Key::LeftControl) && Input::isKeyPressed(Key::O)) {
            scene.clear();
            SceneSerializer::load(scene, "scene.json");
            printf("Scene loaded!\n");
        }
        
        Input::update();
        scene.onUpdate(dt);

        ImVec2 viewportSize = editorLayer.GetViewportSize();
        unsigned int vpWidth = static_cast<unsigned int>(viewportSize.x);
        unsigned int vpHeight = static_cast<unsigned int>(viewportSize.y);

        if (vpWidth > 0 && vpHeight > 0) {
            viewportFramebuffer.Resize(vpWidth, vpHeight);

            if (scene.getActiveCamera()) {
                scene.getActiveCamera()->aspect =
                    static_cast<float>(vpWidth) / static_cast<float>(vpHeight);
            }

            viewportFramebuffer.Bind();
            renderer.render(scene);
            viewportFramebuffer.Unbind();

            editorLayer.SetViewportTexture(viewportFramebuffer.GetColorAttachment());
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        int windowWidth = 0;
        int windowHeight = 0;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        imguiLayer.Begin();
        editorLayer.OnUIRender();
        imguiLayer.End();

        glfwSwapBuffers(window);
    }

    imguiLayer.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
