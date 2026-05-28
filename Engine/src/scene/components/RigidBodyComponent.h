#pragma once

#include "Component.h"
#include <physics/body/PhysicsBody.h>
#include <memory>

using namespace physx;

enum RigidBodyType { Static, Dynamic, Kinematic };

class RigidBodyComponent : public Component {
public:
	RigidBodyComponent(RigidBodyType type = Dynamic,
		std::shared_ptr<PhysicsMaterial> material = nullptr,
		std::shared_ptr<CollisionShape> shape = nullptr)
		: type(type), material(material), shape(shape) {}
	~RigidBodyComponent() = default;

	void onAttach() override;
	void onDetach() override;

	void pushToWorld();
	void pullFromWorld();

	PhysicsBody* getBody() { return body.get(); }
	RigidBodyType getType() const { return type; }

private:
	glm::vec3 lastScale = glm::vec3(1.0f);

	void recookShape();

	RigidBodyType type;
	std::unique_ptr<PhysicsBody> body;
	std::shared_ptr<PhysicsMaterial> material;
	std::shared_ptr<CollisionShape> shape;
};