#include "AssetManager.h"

#include <string>

#include <renderer/ObjLoader.h>
#include <renderer/PrimitiveFactory.h>
#include <renderer/ShaderProgram.h>
#include <renderer/Texture.h>

std::unordered_map<std::string, std::shared_ptr<Mesh>> AssetManager::meshes;
std::unordered_map<std::string, LoadedModel> AssetManager::models;
std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> AssetManager::shaders;
std::unordered_map<std::string, std::shared_ptr<Texture>> AssetManager::textures;

std::shared_ptr<Mesh> AssetManager::getMesh(const std::string& name) {
    auto it = meshes.find(name);
    if (it != meshes.end())
        return it->second;

    std::shared_ptr<Mesh> mesh;

    if (name == "cube")
        mesh = PrimitiveFactory::createCube();
    else if (name == "sphere")
        mesh = PrimitiveFactory::createSphere();
    else if (name == "plane")
        mesh = PrimitiveFactory::createPlane();
    else
        mesh = ObjLoader::load(name);

    mesh->setName(name);
    meshes[name] = mesh;
    return mesh;
}

std::shared_ptr<ShaderProgram> AssetManager::getShader(const std::string& name) {
    auto it = shaders.find(name);
    if (it != shaders.end())
        return it->second;

    auto shader = std::make_shared<ShaderProgram>(name);

    shader->setName(name);
    shaders[name] = shader;
    return shader;
}

void AssetManager::clear() {
    meshes.clear();
    shaders.clear();
}

std::shared_ptr<Texture> AssetManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end())
        return it->second;
    std::string model_path = (std::filesystem::current_path() / "assets" / "models").string();
    auto tex = std::make_shared<Texture>(model_path + name);
    tex->setName(name);
    textures[name] = tex;
    return tex;
}

LoadedModel AssetManager::getModel(const std::string& name) {
    auto it = models.find(name);
    if (it != models.end())
        return it->second;

    auto model = ObjLoader::loadModel(name);
    model.mesh->setName(name);
    models[name] = model;
    meshes[name] = model.mesh;

    return model;
}