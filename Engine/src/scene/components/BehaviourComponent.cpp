#include <scene/components/BehaviourComponent.h>

#include <persistance/ComponentFactory.h>
#include <property/PropertySerializer.h>
#include <persistance/Archive.h>

REGISTER(BehaviourComponent);

void BehaviourComponent::serialize(Archive& arch) const {
    PropertySerializer::serialize(*this, arch);
}

void BehaviourComponent::deserialize(const Archive& arch) {
    PropertySerializer::deserialize(*this, arch);
}