#include "DemoScene.h"

#include <scene/Scene.h>
#include <scene/Entity.h>

#include <scene/components/TransformComponent.h>
#include <scene/components/CameraComponent.h>
#include <scene/components/MeshComponent.h>
#include <scene/components/MaterialComponent.h>
#include <scene/components/DirectionalLightComponent.h>
#include <scene/components/PointLightComponent.h>

#include <renderer/PrimitiveFactory.h>
#include <renderer/Mesh.h>
#include <renderer/ShaderProgram.h>
#include <renderer/Material.h>

#include <glm/glm.hpp>

#include "scripts/Rotator.h"

namespace DemoScene
{
    void create(Scene& scene)
    {
        auto shader = std::make_shared<ShaderProgram>("basic_lit.vert", "basic_lit.frag");
        auto materialRed = std::make_shared<Material>(shader, glm::vec3{ 1.0f, 0.0f, 0.2f });
        auto materialBlue = std::make_shared<Material>(shader, glm::vec3{ 0.0f, 0.0f, 1.0f });
        auto materialGray = std::make_shared<Material>(shader);

        auto planeMesh = PrimitiveFactory::createPlane();
        auto cubeMesh = PrimitiveFactory::createCube();

        Entity& cameraEntity = scene.createEntity("Main Camera");
        CameraComponent& camera = cameraEntity.addComponent<CameraComponent>(45.0f, 1000.0f / 800.0f, 0.1f, 100.0f);

        cameraEntity.getTransform().setPosition(glm::vec3(0.0f, 2.0f, 9.0f));
        cameraEntity.getTransform().rotate(glm::vec3{ glm::radians(-15.0f), 0.0f, 0.0f });

        scene.setActiveCamera(&camera);

        Entity& sun = scene.createEntity("Sun");
        auto& dirLight = sun.addComponent<DirectionalLightComponent>();
        dirLight.direction = glm::vec3(-1.0f, -1.0f, -0.3f);
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

        ground.getTransform().setScale(glm::vec3(8.0f, 1.0f, 8.0f));

        Entity& cube1 = scene.createEntity("Cube A");
        auto& cube1Mesh = cube1.addComponent<MeshComponent>(cubeMesh);
        auto& cube1Mat = cube1.addComponent<MaterialComponent>(materialRed);

        cube1.getTransform().setPosition(glm::vec3(-1.5f, 0.5f, 0.0f));

        auto& rotator = cube1.addComponent<Rotator>();

        Entity& cube2 = scene.createEntity("Cube B");
        auto& cube2Mesh = cube2.addComponent<MeshComponent>(cubeMesh);
        auto& cube2Mat = cube2.addComponent<MaterialComponent>(materialBlue);

        cube2.getTransform().setPosition(glm::vec3(1.5f, 0.5f, 0.0f));
        cube2.getTransform().rotate(glm::vec3(0.0f, 0.0f, 0.0f));
    }
}