#pragma once

#include <scene/components/Component.h>

#include <memory>

class Mesh;
class Archive;

class MeshComponent : public Component {
public:
	MeshComponent(std::shared_ptr<Mesh> mesh = nullptr) : mesh(mesh) {}

	std::shared_ptr<Mesh> mesh;

	void setMesh(std::shared_ptr<Mesh> mesh) { this->mesh = mesh; }

	void serialize(Archive& arch) const override;
	void deserialize(const Archive& arch) override;
};