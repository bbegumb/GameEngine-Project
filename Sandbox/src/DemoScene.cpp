#include "DemoScene.h"

#include <scene/Scene.h>
#include <scene/Entity.h>

#include <scene/components/TransformComponent.h>
#include <scene/components/CameraComponent.h>
#include <scene/components/MeshComponent.h>
#include <scene/components/MaterialComponent.h>
#include <scene/components/DirectionalLightComponent.h>
#include <scene/components/PointLightComponent.h>
#include <scene/components/RigidBodyComponent.h>

#include <renderer/PrimitiveFactory.h>
#include <renderer/Mesh.h>
#include <renderer/ShaderProgram.h>
#include <renderer/Material.h>
#include <renderer/ObjLoader.h>

#include <glm/glm.hpp>

#include "scripts/Rotator.h"

namespace DemoScene
{
    void createScene1(Scene& scene)
    {
        auto shader = std::make_shared<ShaderProgram>("lit.vert", "lit.frag");
        auto materialRed = std::make_shared<Material>(shader, glm::vec3{ 1.0f, 0.0f, 0.2f });
        auto materialBlue = std::make_shared<Material>(shader, glm::vec3{ 0.0f, 0.0f, 1.0f });
        auto materialGray = std::make_shared<Material>(shader);
        auto materialBunny = std::make_shared<Material>(shader, glm::vec3{0.9f, 0.85f, 0.75f});

        auto planeMesh = PrimitiveFactory::createPlane();
        auto cubeMesh = PrimitiveFactory::createCube();
        auto bunnyMesh = ObjLoader::load("bunny.obj");

        Entity& cameraEntity = scene.createEntity("Main Camera");
        CameraComponent& camera = cameraEntity.addComponent<CameraComponent>(70.0f, 1000.0f / 800.0f, 0.1f, 100.0f);

        cameraEntity.getTransform().setPosition(glm::vec3(0.0f, 2.0f, 6.0f));
        cameraEntity.getTransform().rotate(glm::vec3{ glm::radians(-15.0f), 0.0f, 0.0f });

        scene.setActiveCamera(&camera);

        Entity& sun = scene.createEntity("Sun");
        auto& dirLight = sun.addComponent<DirectionalLightComponent>();
        sun.getTransform().setRotation(glm::vec3(glm::radians(-45.0f), glm::radians(-30.0f), 0.0f));
        dirLight.color = glm::vec3(1.0f, 0.95f, 0.75f);
        dirLight.ambientStrength = 0.2f;
        dirLight.diffuseStrength = 0.8f;
        dirLight.specularStrength = 0.4f;

        Entity& lamp = scene.createEntity("Lamp");
        auto& pointLight = lamp.addComponent<PointLightComponent>();
        lamp.getTransform().setPosition(glm::vec3(2.0f, 2.0f, 2.0f));

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

        ground.getTransform().setScale(glm::vec3(8.0f, 0.01f, 8.0f));

        ground.addComponent<RigidBodyComponent>(Kinematic);

        Entity& cube1 = scene.createEntity("Cube A");
        auto& cube1Mesh = cube1.addComponent<MeshComponent>(cubeMesh);
        auto& cube1Mat = cube1.addComponent<MaterialComponent>(materialRed);

        cube1.getTransform().setPosition(glm::vec3(-1.5f, 5.0f, 0.0f));

        cube1.addComponent<RigidBodyComponent>(Dynamic);

        Entity& cube2 = scene.createEntity("Cube2");
        auto& cube2Mesh = cube2.addComponent<MeshComponent>(cubeMesh);
        auto& cube2Mat = cube2.addComponent<MaterialComponent>(materialBlue);

        cube2.getTransform().setPosition(glm::vec3(1.5f, 0.5f, 0.0f));

        cube2.addComponent<RigidBodyComponent>(Dynamic);

        Entity& bunny = scene.createEntity("Bunny");
        auto& bunnyMeshC = bunny.addComponent<MeshComponent>(bunnyMesh);
        auto& bunnyMat = bunny.addComponent<MaterialComponent>(materialBunny);
        bunny.addComponent<Rotator>();
        bunny.getTransform().setScale(glm::vec3{ 15.0f, 15.0f, 15.0f });

        bunny.getTransform().setPosition(glm::vec3(0.0f, 1.15f, 2.0f));
        bunny.getTransform().rotate(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    void createScene2(Scene& scene) {
        auto shader = std::make_shared<ShaderProgram>("lit.vert", "lit.frag");

        auto materialCar = std::make_shared<Material>(shader, glm::vec3{ 0.2f, 0.05f, 0.5f });
        auto materialGround = std::make_shared<Material>(shader, glm::vec3{ 0.3f, 0.3f, 0.3f });

        auto carMesh = ObjLoader::load("bmw.obj");
        auto planeMesh = PrimitiveFactory::createPlane();

        Entity& cameraEntity = scene.createEntity("Main Camera");
        CameraComponent& camera = cameraEntity.addComponent<CameraComponent>(
            70.0f, 1000.0f / 800.0f, 0.1f, 100.0f);
        cameraEntity.getTransform().setPosition(glm::vec3(0.0f, 3.0f, 8.0f));
        cameraEntity.getTransform().rotate(glm::vec3{ glm::radians(-15.0f), 0.0f, 0.0f });
        scene.setActiveCamera(&camera);

        Entity& sun = scene.createEntity("Sun");
        auto& dirLight = sun.addComponent<DirectionalLightComponent>();
        sun.getTransform().setRotation(glm::vec3(glm::radians(-45.0f), glm::radians(-30.0f), 0.0f));
        dirLight.color = glm::vec3(1.0f, 0.95f, 0.75f);
        dirLight.ambientStrength = 0.25f;
        dirLight.diffuseStrength = 0.9f;
        dirLight.specularStrength = 0.6f;

        Entity& ground = scene.createEntity("Ground");
        ground.addComponent<MeshComponent>(planeMesh);
        ground.addComponent<MaterialComponent>(materialGround);
        ground.getTransform().setScale(glm::vec3(15.0f, 1.0f, 15.0f));

        Entity& car = scene.createEntity("BMW");
        car.addComponent<MeshComponent>(carMesh);
        car.addComponent<MaterialComponent>(materialCar);
        car.getTransform().setPosition(glm::vec3(0.0f, 1.4f, 1.0f));
        car.getTransform().setScale(glm::vec3(0.015f, 0.015f, 0.015f));
        car.addComponent<Rotator>();
    }
}