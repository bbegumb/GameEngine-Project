#pragma once

#include "Component.h"
#include <PxPhysicsAPI.h>

using namespace physx;

enum RigidbodyType { Static, Dynamic, Kinematic };

class RigidBodyComponent : public Component {
public:
	RigidBodyComponent(RigidbodyType type) : type(type) {}
	~RigidBodyComponent();

	void init(); // Not called at component creation, but at the end of Entity.addComponent
	void cleanup();
	void sync();

	void onAttach();
	void onDetach();
private:
	RigidbodyType type;
	PxRigidActor* actor = nullptr;
	PxMaterial* mat = nullptr;
};