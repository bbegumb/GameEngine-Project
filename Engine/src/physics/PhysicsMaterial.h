#pragma once

#include <memory>

namespace physx {
	class PxMaterial;
}

class PhysicsWorld;

class PhysicsMaterial {
public:
	PhysicsMaterial(float staticFriction = 0.5f, float dynamicFriction = 0.5f, float restitution = 0.01f)
		: staticFriction(staticFriction), dynamicFriction(dynamicFriction), restitution(restitution) {}
	~PhysicsMaterial();

	physx::PxMaterial* getOrCreate(PhysicsWorld* world = nullptr);

private:
	float staticFriction;
	float dynamicFriction;
	float restitution;

	physx::PxMaterial* pxMat = nullptr;
};