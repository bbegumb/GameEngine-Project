#pragma once

#include <json.hpp>

class Serializable {
public:
	~Serializable() = default;
	virtual void serialize(nlohmann::json& j) const = 0;
	virtual void deserialize(const nlohmann::json& j) = 0;
};