#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <scene/components/Component.h>

class TransformComponent : public Component {
public:
	glm::mat4 getMatrix() const;

	void translate(const glm::vec3& delta);
	void setPosition(const glm::vec3& pos);

	void setRotation(const glm::vec3& eulerAngles);
	void setRotation(const glm::quat& quat);
	void rotate(const glm::vec3& eulerDelta);
	void rotateAroundAxis(const glm::vec3& axis, float angle);

	void setScale(const glm::vec3& scale);
	void scaleBy(const glm::vec3& factor);

	glm::mat3 getRotationMatrix() const;

	glm::vec3 forward() const;
	glm::vec3 right() const;
	glm::vec3 up() const;

	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	glm::quat getRotationQuat() const;
	glm::vec3 getScale() const;

private:
	mutable bool isMatrixValid = false;
	mutable glm::mat4 modelMatrix;

	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
};