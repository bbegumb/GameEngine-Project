#pragma once

#include <scene/components/BehaviourComponent.h>

class Rotator : public BehaviourComponent {
	float timeElapsed = 0.0f;
	float x = 0.0f, y = 0.0f, z = 0.0f;

	void onUpdate(float dt) override;
};