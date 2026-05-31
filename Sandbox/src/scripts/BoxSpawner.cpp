#include "BoxSpawner.h"

#include <scene/Entity.h>
#include <scene/components/MeshComponent.h>
#include <scene/components/MaterialComponent.h>
#include <scene/components/RigidBodyComponent.h>
#include <renderer/Material.h>
#include <core/Input.h>
#include <physics/body/DynamicPhysicsBody.h>
#include <core/AssetManager.h>

void BoxSpawner::onUpdate(float dt) {
    static auto shader = AssetManager::getShader("lit");
    static auto cubeMesh = AssetManager::getMesh("cube");
    static auto sphereMesh = AssetManager::getMesh("sphere");

    auto randomColor = []() {
        return glm::vec3(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX);
        };

    auto randomOffset = []() {
        return glm::vec3(
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 3.0f,
            0.0f,
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 3.0f);
        };

    glm::vec3 spawnPos = owner->getTransform().getPosition() + glm::vec3(0, 5, 0);

    if (Input::isKeyPressed(Key::Space)) {
        auto material = std::make_shared<Material>(shader, randomColor());
        Entity& box = owner->getScene().createEntity("Box_" + std::to_string(count++));
        box.addComponent<MeshComponent>(cubeMesh);
        box.addComponent<MaterialComponent>(material);
        box.getTransform().setPosition(spawnPos + randomOffset());
        box.addComponent<RigidBodyComponent>(Dynamic);
    }

    if (Input::isKeyPressed(Key::B)) {
        auto material = std::make_shared<Material>(shader, randomColor());
        Entity& ball = owner->getScene().createEntity("Ball_" + std::to_string(count++));
        ball.addComponent<MeshComponent>(sphereMesh);
        ball.addComponent<MaterialComponent>(material);
        ball.getTransform().setPosition(spawnPos + randomOffset());
        ball.addComponent<RigidBodyComponent>(Dynamic);
    }

    if (Input::isKeyPressed(Key::F)) {
        for (auto& entity : owner->getScene().getEntities()) {
            auto* rb = entity->getComponent<RigidBodyComponent>();
            if (rb && rb->getType() == Dynamic) {
                auto* body = static_cast<DynamicPhysicsBody*>(rb->getBody());
                body->addImpulse(glm::vec3(0, 15, 0));
            }
        }
    }

    if (Input::isKeyPressed(Key::R)) {
        auto& entities = owner->getScene().getEntities();
        for (int i = static_cast<int>(entities.size()) - 1; i >= 0; i--) {
            if (entities[i]->getName().find("Box_") == 0 ||
                entities[i]->getName().find("Ball_") == 0) {
                owner->getScene().removeEntity(entities[i].get());
            }
        }
        count = 0;
    }

    if (Input::isKeyDown(Key::G)) {
        for (auto& entity : owner->getScene().getEntities()) {
            auto* rb = entity->getComponent<RigidBodyComponent>();
            if (rb && rb->getType() == Dynamic) {
                auto* body = static_cast<DynamicPhysicsBody*>(rb->getBody());
                glm::vec3 randomForce(
                    (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f,
                    static_cast<float>(rand()) / RAND_MAX * 10.0f,
                    (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 20.0f);
                body->addForce(randomForce);
            }
        }
    }
}