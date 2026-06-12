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
		std::shared_ptr<CollisionShape> shape = nullptr,
		bool forceConvex = false)
		: type(type), material(material), shape(shape), forceConvex(forceConvex) {}
	~RigidBodyComponent() = default;

	bool onAttach() override;
	void onDetach() override;

	void serialize(nlohmann::json& j) const override;
	void deserialize(const nlohmann::json& j) override;

	void pushToWorld();
	void pullFromWorld();

	void setLinearDamping(float damping);
	void setAngularDamping(float damping);

	PhysicsBody* getBody() { return body.get(); }
	RigidBodyType getType() const { return type; }

	bool getForceConvex() { return forceConvex; }
	bool getUseTriangleMesh() { return useTriangleMesh; }

	void setType(RigidBodyType newType);
	void setForceConvex(bool value) {
		if (forceConvex == value) return;

		forceConvex = value;
		recookShape();
	}

	void setUseTriangleMesh(bool value) {
		if (useTriangleMesh == value) return;

		useTriangleMesh = value;
		recookShape();
	}

	std::shared_ptr<PhysicsMaterial> getMaterial() const { return material; }

private:
	glm::vec3 lastScale = glm::vec3(1.0f);

	void recookShape();
	void rebuild();

	bool forceConvex = false;
	bool useTriangleMesh = false;
	float linearDamping = 0.2f;
	float angularDamping = 0.1f;

	RigidBodyType type;
	std::unique_ptr<PhysicsBody> body;
	std::shared_ptr<PhysicsMaterial> material;
	std::shared_ptr<CollisionShape> shape;
};