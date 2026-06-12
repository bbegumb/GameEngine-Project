#include "BehaviourComponent.h"

#include <persistance/ComponentFactory.h>
#include <property/PropertySerializer.h>

REGISTER(BehaviourComponent);

void BehaviourComponent::serialize(nlohmann::json& j) const {
    PropertySerializer::serialize(*this, j);
}

void BehaviourComponent::deserialize(const nlohmann::json& j) {
    PropertySerializer::deserialize(*this, j);
}