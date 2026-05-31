cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/glfw)

find_package(OpenGL REQUIRED)

add_executable(ReflectionGenerator
    Tools/ReflectionGenerator.cpp
)

set(GENERATED_REFLECTION_CPP
    ${CMAKE_BINARY_DIR}/ReflectionGenerated.cpp
)

add_custom_command(
    OUTPUT ${GENERATED_REFLECTION_CPP}
    COMMAND ReflectionGenerator
            ${CMAKE_SOURCE_DIR}/Engine/src/scene/components
            ${GENERATED_REFLECTION_CPP}
            ${CMAKE_SOURCE_DIR}
    DEPENDS ReflectionGenerator
    COMMENT "Generating reflection code"
)

file(GLOB SCENE_SRC CONFIGURE_DEPENDS Engine/src/scene/*.cpp)
file(GLOB SCENE_COMPONENT_SRC CONFIGURE_DEPENDS Engine/src/scene/components/*.cpp)
file(GLOB RENDERER_SRC CONFIGURE_DEPENDS Engine/src/renderer/*.cpp)
file(GLOB SCRIPTS_SRC CONFIGURE_DEPENDS Sandbox/src/scripts/*.cpp)

set(IMGUI_SRC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(Engine
    ${SCENE_SRC}
    ${SCENE_COMPONENT_SRC}
    ${RENDERER_SRC}
    ${IMGUI_SRC}
    ${GENERATED_REFLECTION_CPP}
    Engine/src/glad.c
    Engine/src/gui/ImGuiLayer.cpp
    Engine/src/gui/EditorLayer.cpp
    Engine/src/gui/ViewportFramebuffer.cpp
    Engine/src/controller/SceneController.cpp
    Engine/src/controller/EntityController.cpp
    Engine/src/controller/TransformController.cpp
    Engine/src/gui/Gizmohelp.cpp
    Engine/src/renderer/LineRenderer.cpp
    Engine/src/controller/EditorSelectionController.cpp
)

target_include_directories(Engine PUBLIC
    glad/include
    external/glm
    external/imgui
    external/imgui/backends
    include
    Engine/src
)

target_link_libraries(Engine PUBLIC
    glfw
    OpenGL::GL
)

target_compile_definitions(Engine PUBLIC
    SHADER_PATH="${CMAKE_SOURCE_DIR}/assets/shaders/"
    MODEL_PATH="${CMAKE_SOURCE_DIR}/assets/models/"
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

add_executable(Sandbox
    ${SCRIPTS_SRC}
    Sandbox/src/main.cpp
    Sandbox/src/DemoScene.cpp
)

target_include_directories(Sandbox PUBLIC
    Engine/src
    Sandbox/src
    Engine/src/gui
)

target_link_libraries(Sandbox PRIVATE Engine)
