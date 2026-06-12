#pragma once

#include <memory>
#include <unordered_map>

#include <string>

#include <renderer/ObjLoader.h>

class Mesh;
class LoadedModel;
class ShaderProgram;
class Texture;

class AssetManager {
public:
    static std::shared_ptr<Mesh> getMesh(const std::string& name);
    static LoadedModel getModel(const std::string& name);
    static std::shared_ptr<ShaderProgram> getShader(const std::string& name);
    static std::shared_ptr<Texture> getTexture(const std::string& name);

    static void clear();

private:
    static std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
    static std::unordered_map<std::string, LoadedModel> models;
    static std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaders;
    static std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};