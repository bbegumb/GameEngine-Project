#pragma once

#include <string>

class Scene;

class SceneSerializer {
public:
    static void save(const Scene& scene, const std::string& filepath);
    static void load(Scene& scene, const std::string& filepath);
};