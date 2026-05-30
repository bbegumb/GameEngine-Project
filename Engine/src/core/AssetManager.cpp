#include "AssetManager.h"

#include <string>

#include <renderer/ObjLoader.h>
#include <renderer/PrimitiveFactory.h>
#include <renderer/ShaderProgram.h>

std::unordered_map<std::string, std::shared_ptr<Mesh>> AssetManager::meshes;
std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> AssetManager::shaders;

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