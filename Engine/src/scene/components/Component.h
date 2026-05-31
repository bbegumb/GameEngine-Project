#pragma once

#include <persistance/Serializable.h>

class Entity;

class Component : public Serializable {
	friend class Entity;
public:
	virtual ~Component() = default;

	Entity* getEntity() const { return owner; }

	virtual bool onAttach() { return true; }
	virtual void onDetach() {}

	void serialize(nlohmann::json& j) const override {}
	void deserialize(const nlohmann::json& j) override {}
protected:
	Entity* owner = nullptr;
};
