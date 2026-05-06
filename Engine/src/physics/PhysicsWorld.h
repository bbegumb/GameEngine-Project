#pragma once

#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace physx;
using namespace glm;

inline PxVec3 toPx(const vec3& v) { return PxVec3(v.x, v.y, v.z); }
inline vec3 toGlm(const PxVec3& v) { return vec3(v.x, v.y, v.z); }
inline PxQuat toPx(const quat& q) { return PxQuat(q.x, q.y, q.z, q.w); }
inline quat toGlm(const PxQuat& q) { return quat(q.x, q.y, q.z, q.w); }

class PhysicsWorld {
public:
	void init();
	void shutdown();
	void step(float dt);

	void addActor(PxActor& actor);
	void removeActor(PxActor& actor);

	PxPhysics* getPhysics() const { return gPhysics; }
	PxScene* getScene() const { return gScene; }
private:
	PxDefaultAllocator gAllocator;
	PxDefaultErrorCallback gErrorCallback;

	PxFoundation* gFoundation = nullptr;
	PxPhysics* gPhysics = nullptr;
	PxScene* gScene = nullptr;
	PxDefaultCpuDispatcher* gDispatcher = nullptr;
};