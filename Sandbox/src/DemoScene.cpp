#include "DemoScene.h"

#include <core/AssetManager.h>

#include <scene/Scene.h>
#include <scene/Entity.h>

#include <scene/components/TransformComponent.h>
#include <scene/components/CameraComponent.h>
#include <scene/components/MeshComponent.h>
#include <scene/components/MaterialComponent.h>
#include <scene/components/DirectionalLightComponent.h>
#include <scene/components/PointLightComponent.h>
#include <scene/components/RigidBodyComponent.h>

#include <renderer/Mesh.h>
#include <renderer/ShaderProgram.h>
#include <renderer/Material.h>
#include <renderer/ObjLoader.h>
#include <renderer/Texture.h>

#include <glm/glm.hpp>

#include "scripts/Rotator.h"
#include "scripts/BoxSpawner.h"

namespace DemoScene
{
    void createScene1(Scene& scene)
    {
        auto shader = AssetManager::getShader("lit");
        auto materialRed = std::make_shared<Material>(shader, glm::vec3{ 1.0f, 0.0f, 0.2f });
        auto materialBlue = std::make_shared<Material>(shader, glm::vec3{ 0.0f, 0.0f, 1.0f });
        auto materialGray = std::make_shared<Material>(shader);
        auto materialBunny = std::make_shared<Material>(shader, glm::vec3{0.9f, 0.85f, 0.75f});
        auto woodTex = AssetManager::getTexture("wood.jpg");

        materialGray->specularReflectance = 0.0f;

        auto planeMesh = AssetManager::getMesh("plane");
        auto cubeMesh = AssetManager::getMesh("cube");
        auto sphereMesh = AssetManager::getMesh("sphere");
        auto bunnyMesh = AssetManager::getMesh("bunny.obj");

        Entity& cameraEntity = scene.createEntity("Main Camera");
        CameraComponent& camera = cameraEntity.addComponent<CameraComponent>(70.0f, 1000.0f / 800.0f, 0.1f, 100.0f);

        cameraEntity.transform.setPosition(glm::vec3(0.0f, 2.0f, 6.0f));
        cameraEntity.transform.rotate(glm::vec3{ glm::radians(-15.0f), 0.0f, 0.0f });

        scene.setActiveCamera(&camera);

        Entity& sun = scene.createEntity("Sun");
        auto& dirLight = sun.addComponent<DirectionalLightComponent>();
        sun.transform.setRotation(glm::vec3(glm::radians(-45.0f), glm::radians(-30.0f), 0.0f));
        dirLight.color = glm::vec3(1.0f, 0.95f, 0.75f);
        dirLight.ambientStrength = 0.2f;
        dirLight.diffuseStrength = 0.8f;
        dirLight.specularStrength = 0.4f;

        Entity& lamp = scene.createEntity("Lamp");
        auto& pointLight = lamp.addComponent<PointLightComponent>();
        lamp.transform.setPosition(glm::vec3(2.0f, 2.0f, 2.0f));

        pointLight.color = glm::vec3(1.0f, 0.9f, 0.7f);
        pointLight.ambientStrength = 0.05f;
        pointLight.diffuseStrength = 1.2f;
        pointLight.specularStrength = 1.0f;
        pointLight.constant = 1.0f;
        pointLight.linear = 0.09f;
        pointLight.quadratic = 0.032f;

        Entity& ground = scene.createEntity("Ground");
        auto& groundMesh = ground.addComponent<MeshComponent>(planeMesh);
        auto& groundMat = ground.addComponent<MaterialComponent>(materialGray);
        groundMat.getMaterial()->diffuseTexture = woodTex;

        ground.transform.setPosition(glm::vec3(0.0f, -0.5f, 0.0f));
        ground.transform.setScale(glm::vec3(8.0f, 0.1f, 8.0f));

        ground.addComponent<RigidBodyComponent>(Kinematic);

        Entity& cube1 = scene.createEntity("Cube1");
        auto& cube1Mesh = cube1.addComponent<MeshComponent>(cubeMesh);
        auto& cube1Mat = cube1.addComponent<MaterialComponent>(materialRed);

        cube1.transform.setPosition(glm::vec3(-1.5f, 5.0f, 0.0f));
        cube1.addComponent<RigidBodyComponent>();

        Entity& cube2 = scene.createEntity("Cube2");
        auto& cube2Mesh = cube2.addComponent<MeshComponent>(cubeMesh);
        auto& cube2Mat = cube2.addComponent<MaterialComponent>(materialBlue);

        cube2.transform.setPosition(glm::vec3(1.5f, 0.5f, 0.0f));
        cube2.addComponent<RigidBodyComponent>();

        Entity& sphere1 = scene.createEntity("Sphere1");
        auto& sphere1Mesh = sphere1.addComponent<MeshComponent>(sphereMesh);
        auto& sphere1Mat = sphere1.addComponent<MaterialComponent>(materialBlue);

        sphere1.transform.setPosition(glm::vec3(-1.5f, 0.5f, 0.0f));
        sphere1.addComponent<RigidBodyComponent>(Dynamic, nullptr, nullptr, true);

        Entity& bunny = scene.createEntity("Bunny");
        auto& bunnyMeshC = bunny.addComponent<MeshComponent>(bunnyMesh);
        auto& bunnyMat = bunny.addComponent<MaterialComponent>(materialBunny);
        bunny.addComponent<Rotator>();
        bunny.transform.setScale(glm::vec3{ 15.0f, 15.0f, 15.0f });

        bunny.transform.setPosition(glm::vec3(0.0f, 1.15f, 2.0f));
        bunny.transform.rotate(glm::vec3(0.0f, 0.0f, 0.0f));

        bunny.addComponent<RigidBodyComponent>();
    }

    void createScene2(Scene& scene) {
        auto shader = AssetManager::getShader("lit");

        auto materialCar = std::make_shared<Material>(shader, glm::vec3{ 0.2f, 0.05f, 0.5f });
        auto materialGround = std::make_shared<Material>(shader, glm::vec3{ 0.3f, 0.3f, 0.3f });

        auto carModel = AssetManager::getModel("bmw.obj");
        auto planeMesh = AssetManager::getMesh("plane");

        Entity& cameraEntity = scene.createEntity("Main Camera");
        CameraComponent& camera = cameraEntity.addComponent<CameraComponent>(
            70.0f, 1000.0f / 800.0f, 0.1f, 100.0f);
        cameraEntity.transform.setPosition(glm::vec3(0.0f, 3.0f, 8.0f));
        cameraEntity.transform.rotate(glm::vec3{ glm::radians(-15.0f), 0.0f, 0.0f });
        scene.setActiveCamera(&camera);

        Entity& sun = scene.createEntity("Sun");
        auto& dirLight = sun.addComponent<DirectionalLightComponent>();
        sun.transform.setRotation(glm::vec3(glm::radians(-45.0f), glm::radians(-30.0f), 0.0f));
        dirLight.color = glm::vec3(1.0f, 0.95f, 0.75f);
        dirLight.ambientStrength = 0.25f;
        dirLight.diffuseStrength = 0.9f;
        dirLight.specularStrength = 0.6f;

        Entity& ground = scene.createEntity("Ground");
        ground.addComponent<MeshComponent>(planeMesh);
        ground.addComponent<MaterialComponent>(materialGround);
        ground.transform.setScale(glm::vec3(15.0f, 1.0f, 15.0f));
        ground.addComponent<RigidBodyComponent>(Kinematic);

        Entity& car = scene.createEntity("BMW");
        car.addComponent<MeshComponent>(carModel.mesh);
        car.addComponent<MaterialComponent>(carModel.materials);
        car.transform.setPosition(glm::vec3(0.0f, 1.4f, 1.0f));
        car.transform.setScale(glm::vec3(0.015f, 0.015f, 0.015f));
        car.addComponent<Rotator>();
        car.addComponent<RigidBodyComponent>();
    }

    void createScene3(Scene& scene)
    {
        auto shader = AssetManager::getShader("lit");
        auto materialRed = std::make_shared<Material>(shader, glm::vec3{ 1.0f, 0.0f, 0.2f });
        auto materialBlue = std::make_shared<Material>(shader, glm::vec3{ 0.0f, 0.0f, 1.0f });
        auto materialGray = std::make_shared<Material>(shader);
        auto materialBunny = std::make_shared<Material>(shader, glm::vec3{ 0.9f, 0.85f, 0.75f });
        auto woodTex = AssetManager::getTexture("wood.jpg");

        materialGray->specularReflectance = 0.0f;

        auto planeMesh = AssetManager::getMesh("plane");
        auto cubeMesh = AssetManager::getMesh("cube");
        auto sphereMesh = AssetManager::getMesh("sphere");
        auto bunnyMesh = AssetManager::getMesh("bunny.obj");

        Entity& cameraEntity = scene.createEntity("Main Camera");
        CameraComponent& camera = cameraEntity.addComponent<CameraComponent>(70.0f, 1000.0f / 800.0f, 0.1f, 100.0f);

        cameraEntity.transform.setPosition(glm::vec3(0.0f, 2.0f, 6.0f));
        cameraEntity.transform.rotate(glm::vec3{ glm::radians(-15.0f), 0.0f, 0.0f });

        scene.setActiveCamera(&camera);

        Entity& sun = scene.createEntity("Sun");
        auto& dirLight = sun.addComponent<DirectionalLightComponent>();
        sun.transform.setRotation(glm::vec3(glm::radians(-45.0f), glm::radians(-30.0f), 0.0f));
        dirLight.color = glm::vec3(1.0f, 0.95f, 0.75f);
        dirLight.ambientStrength = 0.2f;
        dirLight.diffuseStrength = 0.8f;
        dirLight.specularStrength = 0.4f;

        Entity& lamp = scene.createEntity("Lamp");
        auto& pointLight = lamp.addComponent<PointLightComponent>();
        lamp.transform.setPosition(glm::vec3(2.0f, 2.0f, 2.0f));

        pointLight.color = glm::vec3(1.0f, 0.9f, 0.7f);
        pointLight.ambientStrength = 0.05f;
        pointLight.diffuseStrength = 1.2f;
        pointLight.specularStrength = 1.0f;
        pointLight.constant = 1.0f;
        pointLight.linear = 0.09f;
        pointLight.quadratic = 0.032f;

        Entity& ground = scene.createEntity("Ground");
        auto& groundMesh = ground.addComponent<MeshComponent>(planeMesh);
        auto& groundMat = ground.addComponent<MaterialComponent>(materialGray);
        groundMat.getMaterial()->diffuseTexture = woodTex;

        ground.transform.setScale(glm::vec3(20.0f, 0.01f, 20.0f));

        ground.addComponent<RigidBodyComponent>(Kinematic);

        Entity& spawner = scene.createEntity("Spawner");
        spawner.addComponent<BoxSpawner>();
    }
}
