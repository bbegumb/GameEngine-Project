#include "Scene.h"

#include <scene/Entity.h>
#include <scene/components/BehaviourComponent.h>
#include <scene/components/RigidBodyComponent.h>

Scene::Scene() {
	physicsWorld.init();
}

Scene::~Scene() {
	entities.clear();
	physicsWorld.shutdown();
}

Entity& Scene::createEntity(const std::string& name) {
	auto entity = std::unique_ptr<Entity>(new Entity(this, name));
	Entity& ref = *entity;

	ref.addComponent<TransformComponent>();

	entities.push_back(std::move(entity));
	return ref;
}

void Scene::onCameraAdded(CameraComponent* camera)
{
	if (activeCamera == nullptr)
		activeCamera = camera;
}

void Scene::onUpdate(float dt) {
	const float fixedPhysicsStep = 1.0f / 60.0f;
	physicsAccumulator += dt;
	colliderCacheTimer += dt;

	if (colliderCacheTimer > 1.0f) {
		physicsWorld.cleanShapeCache();
		colliderCacheTimer -= 1.0f;
	}

	bool isPhysicsLoop = physicsAccumulator >= fixedPhysicsStep;

	if (isPhysicsLoop)
		for (auto& entity : entities) {
			auto* rb = entity->getComponent<RigidBodyComponent>();
			if (rb) rb->pushToWorld();
		}

	while (physicsAccumulator >= fixedPhysicsStep) {
		physicsWorld.step(fixedPhysicsStep);
		physicsAccumulator -= fixedPhysicsStep;
	}

	if (isPhysicsLoop)
		for (auto& entity : entities) {
			auto* rb = entity->getComponent<RigidBodyComponent>();
			if (rb) rb->pullFromWorld();
		}

	for (const auto& entity : entities) {
		for (const auto& component : entity->getComponents()) {
			if (auto* behaviour = dynamic_cast<BehaviourComponent*>(component.get())) {
				behaviour->onUpdate(dt);
			}
		}
	}
}

