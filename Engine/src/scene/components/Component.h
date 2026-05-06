#pragma once

class Entity;

class Component {
	friend class Entity;
public:
	virtual ~Component() = default;

	Entity* getEntity() const { return owner; }

	virtual void onAttach() {}
	virtual void onDetach() {}
protected:
	Entity* owner = nullptr;
};