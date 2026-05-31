#include "SceneSerializer.h"

#include <fstream>
#include <json.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <scene/Scene.h>
#include <scene/Entity.h>
#include <persistance/ComponentFactory.h>

using json = nlohmann::json;

void SceneSerializer::save(const Scene& scene, const std::string& filepath) {
    if (scene.isPlaying) return;

    json j;

    if (scene.getActiveCamera())
        j["scene"]["activeCamera"] = scene.getActiveCamera()->getEntity()->getName();

    j["scene"]["entities"] = json::array();

    for (const auto& entity : scene.getEntities()) {
        json ej;
        entity->serialize(ej);
        j["scene"]["entities"].push_back(ej);
    }

    std::ofstream file(SCENES_PATH + filepath);
    file << j.dump(4);
}

void SceneSerializer::load(Scene& scene, const std::string& filepath) {
    scene.isPlaying = false;

    std::ifstream file(SCENES_PATH + filepath);
    if (!file.is_open()) return;

    json j = json::parse(file);

    for (const auto& ej : j["scene"]["entities"]) {
        Entity& entity = scene.createEntityImmediate(ej["name"]);

        auto pos = ej["transform"]["position"].get<std::vector<float>>();
        auto rot = ej["transform"]["rotation"].get<std::vector<float>>();
        auto scl = ej["transform"]["scale"].get<std::vector<float>>();

        entity.getTransform().setPosition(glm::make_vec3(pos.data()));
        entity.getTransform().setRotation(glm::quat(rot[3], rot[0], rot[1], rot[2]));
        entity.getTransform().setScale(glm::make_vec3(scl.data()));

        for (const auto& cj : ej["components"]) {
            ComponentFactory::create(cj["type"], entity, cj);
        }
    }

    for (const auto& ej : j["scene"]["entities"]) {
        if (ej.contains("parent") && !ej["parent"].is_null()) {
            std::string childName = ej["name"];
            std::string parentName = ej["parent"];

            Entity* child = scene.findEntity(childName);
            Entity* parent = scene.findEntity(parentName);

            if (child && parent)
                child->getTransform().setParent(&parent->getTransform());
        }
    }

    if (j["scene"].contains("activeCamera")) {
        std::string camName = j["scene"]["activeCamera"];
        Entity* camEntity = scene.findEntity(camName);
        if (camEntity) {
            auto* cam = camEntity->getComponent<CameraComponent>();
            if (cam) scene.setActiveCamera(cam);
        }
    }
}