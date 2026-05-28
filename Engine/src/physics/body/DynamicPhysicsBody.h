#pragma once

#include <physics/body/PhysicsBody.h>

class PhysicsWorld;
class PhysicsMaterial;
class CollisionShape;

class DynamicPhysicsBody : public PhysicsBody {
public:
	DynamicPhysicsBody(PhysicsWorld* world, const glm::vec3& pos,
					   const glm::quat& rot, std::shared_ptr<PhysicsMaterial> mat = nullptr,
					   std::shared_ptr<CollisionShape> shape = nullptr, const glm::vec3& scale = glm::vec3(1.0f), bool kinematic = false);
	~DynamicPhysicsBody() = default;

	void setGlobalPose(const glm::vec3& pos, const glm::quat& rot);
	void setKinematicTarget(const glm::vec3& pos, const glm::quat& rot);

	void addForce(const glm::vec3& force);
	void addImpulse(const glm::vec3& impulse);
	void setLinearVelocity(const glm::vec3& v);
	void setAngularVelocity(const glm::vec3& v);

	glm::vec3 getLinearVelocity() const;
	glm::vec3 getAngularVelocity() const;

	bool isKinematic() const { return _isKinematic; }
private:
	bool _isKinematic;
};