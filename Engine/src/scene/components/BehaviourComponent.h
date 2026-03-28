#pragma once

#include <scene/components/Component.h>

class BehaviourComponent : public Component {
public:
	virtual ~BehaviourComponent() = default;

	virtual void onStart() {}
	virtual void onUpdate(float dt) {}
};