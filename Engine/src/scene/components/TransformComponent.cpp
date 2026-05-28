#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 TransformComponent::getMatrix() const {
	if (isMatrixValid) return modelMatrix;

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, position);
	model *= glm::mat4_cast(rotation);
	model = glm::scale(model, scale);

	modelMatrix = model;
	isMatrixValid = true;
	return model;
}

void TransformComponent::translate(const glm::vec3& delta) {
	position += delta;
	isMatrixValid = false;
}

void TransformComponent::setPosition(const glm::vec3& pos) {
	position = pos;
	isMatrixValid = false;
}

void TransformComponent::setRotation(const glm::vec3& eulerAngles) {
	rotation = glm::quat(eulerAngles);
	isMatrixValid = false;
}

void TransformComponent::setRotation(const glm::quat& quat) {
	rotation = quat;
	isMatrixValid = false;
}

void TransformComponent::rotate(const glm::vec3& eulerDelta) {
	glm::quat delta(eulerDelta);
	rotation = delta * rotation;
	isMatrixValid = false;
}

void TransformComponent::rotateAroundAxis(const glm::vec3& axis, float angle) {
	rotation = glm::angleAxis(angle, glm::normalize(axis)) * rotation;
	isMatrixValid = false;
}

void TransformComponent::setScale(const glm::vec3& scale) {
	this->scale = scale;
	isMatrixValid = false;
}

void TransformComponent::scaleBy(const glm::vec3& factor) {
	scale *= factor;
	isMatrixValid = false;
}

glm::mat3 TransformComponent::getRotationMatrix() const
{
	return glm::mat3_cast(rotation);
}

glm::vec3 TransformComponent::forward() const
{
	return glm::normalize(rotation * glm::vec3(0, 0, -1));
}

glm::vec3 TransformComponent::right() const
{
	return glm::normalize(rotation * glm::vec3(1, 0, 0));
}

glm::vec3 TransformComponent::up() const
{
	return glm::normalize(rotation * glm::vec3(0, 1, 0));
}

glm::vec3 TransformComponent::getPosition() const {
	return position;
}

glm::vec3 TransformComponent::getRotation() const {
	return glm::eulerAngles(rotation);
}

glm::quat TransformComponent::getRotationQuat() const {
	return rotation;
}

glm::vec3 TransformComponent::getScale() const {
	return scale;
}