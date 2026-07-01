#pragma once

#include <scene/components/Component.h>

#include <glm/glm.hpp>
#include <memory>

class PhysicsBody;
class PhysicsMaterial;
class CollisionShape;
class Archive;

enum RigidBodyType { Static, Dynamic, Kinematic };

class RigidBodyComponent : public Component {
public:
	RigidBodyComponent(RigidBodyType type = Dynamic,
		std::shared_ptr<PhysicsMaterial> material = nullptr,
		std::shared_ptr<CollisionShape> shape = nullptr,
		bool forceConvex = false);

	~RigidBodyComponent();

	bool onAttach() override;
	void onDetach() override;

	void serialize(Archive& arch) const override;
	void deserialize(const Archive& arch) override;

	void pushToWorld();
	void pullFromWorld();

	RigidBodyType getType() const { return type; }
	void setType(RigidBodyType newType);

	bool getForceConvex() const { return forceConvex; }
	void setForceConvex(bool value);

	bool getUseTriangleMesh() const { return useTriangleMesh; }
	void setUseTriangleMesh(bool value);

	void setGlobalPose(const glm::vec3& pos, const glm::quat& rot, bool autowake = true);
	void setKinematicTarget(const glm::vec3& pos, const glm::quat& rot);

	void addForce(const glm::vec3& force);
	void addForceAtPosition(const glm::vec3& force, const glm::vec3& worldPos);
	void addForceAtLocalPosition(const glm::vec3& force, const glm::vec3& localPos);
	void addImpulse(const glm::vec3& impulse);

	void setLinearVelocity(const glm::vec3& v);
	glm::vec3 getLinearVelocity() const;

	void setAngularVelocity(const glm::vec3& v);
	glm::vec3 getAngularVelocity() const;

	void setLinearDamping(float damping);
	void setAngularDamping(float damping);
	float getLinearDamping() const { return linearDamping; }
	float getAngularDamping() const { return angularDamping; }

	glm::vec3 getWorldPosition() const;
	glm::quat getWorldRotation() const;

	void setStaticFriction(float f);
	void setDynamicFriction(float f);
	void setRestitution(float r);
	float getStaticFriction() const;
	float getDynamicFriction() const;
	float getRestitution() const;

	std::shared_ptr<PhysicsMaterial> getMaterial() const;

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