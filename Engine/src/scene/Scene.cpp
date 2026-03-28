#include "Scene.h"

#include <scene/Entity.h>
#include <scene/components/BehaviourComponent.h>

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
	for (const auto& entity : entities) {
		for (const auto& component : entity->getComponents()) {
			if (auto* behaviour = dynamic_cast<BehaviourComponent*>(component.get())) {
				behaviour->onUpdate(dt);
			}
		}
	}
}

