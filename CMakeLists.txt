cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PHYSX_DIR ${CMAKE_SOURCE_DIR}/external/physx)

add_subdirectory(external/glfw)

find_package(OpenGL REQUIRED)

file(GLOB PHYSICS_SRC CONFIGURE_DEPENDS Engine/src/physics/*.cpp)
file(GLOB SCENE_SRC CONFIGURE_DEPENDS Engine/src/scene/*.cpp)
file(GLOB SCENE_COMPONENT_SRC CONFIGURE_DEPENDS Engine/src/scene/components/*.cpp)
file(GLOB RENDERER_SRC CONFIGURE_DEPENDS Engine/src/renderer/*.cpp)
file(GLOB SCRIPTS_SRC CONFIGURE_DEPENDS Sandbox/src/scripts/*.cpp)
file(GLOB PHYSX_DLLS ${PHYSX_DIR}/PhysX*.dll)

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
    ${PHYSICS_SRC}
    ${SCENE_SRC}
    ${SCENE_COMPONENT_SRC}
    ${RENDERER_SRC}
    ${IMGUI_SRC}
    Engine/src/glad.c
)

target_include_directories(Engine PUBLIC
    glad/include
    external/glm
    ${PHYSX_DIR}/include
    external/imgui
    external/imgui/backends
    include
    Engine/src
)

target_link_libraries(Engine PUBLIC
    glfw
    OpenGL::GL
    ${PHYSX_DIR}/PhysX_64.lib
    ${PHYSX_DIR}/PhysXCommon_64.lib
    ${PHYSX_DIR}/PhysXFoundation_64.lib
    ${PHYSX_DIR}/PhysXExtensions_static_64.lib
    ${PHYSX_DIR}/PhysXCooking_64.lib
    ${PHYSX_DIR}/PhysXPvdSDK_static_64.lib
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
    Sandbox/src/gui/ImGuiLayer.cpp
    Sandbox/src/gui/EditorLayer.cpp
    Sandbox/src/gui/ViewportFramebuffer.cpp
)

add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PHYSX_DLLS}
    $<TARGET_FILE_DIR:Sandbox>
)

target_include_directories(Sandbox PUBLIC
    Engine/src
    Sandbox/src
    Sandbox/src/gui
)

target_link_libraries(Sandbox PRIVATE Engine)
