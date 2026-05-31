#pragma once

class Entity;

class Component {
	friend class Entity;
public:
	virtual ~Component() = default;

	Entity* getEntity() const { return owner; }

protected:
	Entity* owner = nullptr;
};
