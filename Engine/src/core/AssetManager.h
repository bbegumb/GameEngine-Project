#pragma once

#include <memory>
#include <unordered_map>

#include <string>

class Mesh;
class Texture;
class ShaderProgram;

class AssetManager {
public:
    static std::shared_ptr<Mesh> getMesh(const std::string& name);
    static std::shared_ptr<ShaderProgram> getShader(const std::string& name);
    static void clear();

private:
    static std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
    static std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaders;
};