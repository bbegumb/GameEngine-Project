cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/glfw)

find_package(OpenGL REQUIRED)

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
    Engine/src/glad.c
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
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

add_executable(Sandbox
    ${SCRIPTS_SRC}
    Sandbox/src/main.cpp
    Sandbox/src/DemoScene.cpp
    Sandbox/src/gui/ImGuiLayer.cpp
    Sandbox/src/gui/EditorLayer.cpp
    Sandbox/src/gui/ViewportFramebuffer.cpp
)

target_include_directories(Sandbox PUBLIC
    Engine/src
    Sandbox/src
    Sandbox/src/gui
)

target_link_libraries(Sandbox PRIVATE Engine)
