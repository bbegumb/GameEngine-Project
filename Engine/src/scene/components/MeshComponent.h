#pragma once

#include <scene/components/Component.h>
#include <memory>

class Mesh;

class MeshComponent : public Component {
public:
	MeshComponent(std::shared_ptr<Mesh> mesh = nullptr) : mesh(mesh) {}

	std::shared_ptr<Mesh> mesh;

	void setMesh(std::shared_ptr<Mesh> mesh) { this->mesh = mesh; }

	bool onAttach() override;

	void serialize(nlohmann::json& j) const override;
	void deserialize(const nlohmann::json& j) override;
};