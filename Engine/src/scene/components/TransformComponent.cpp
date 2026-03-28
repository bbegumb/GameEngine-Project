#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 TransformComponent::getMatrix() const {
	if (isMatrixValid) return modelMatrix;

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, position);
	model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
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

void TransformComponent::setRotation(const glm::vec3& rot) {
	rotation = rot;
	isMatrixValid = false;
}

void TransformComponent::rotate(const glm::vec3& eulerDelta) {
	rotation += eulerDelta;
	isMatrixValid = false;
}

void TransformComponent::rotateAroundAxis(const glm::vec3& axis, float angle) {
	rotation += axis * angle;
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
	glm::mat4 rot = glm::mat4(1.0f);

	rot = glm::rotate(rot, rotation.x, glm::vec3(1, 0, 0));
	rot = glm::rotate(rot, rotation.y, glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, rotation.z, glm::vec3(0, 0, 1));

	return glm::mat3(rot);
}

glm::vec3 TransformComponent::forward() const
{
	glm::mat4 rot = getRotationMatrix();
	return glm::normalize(glm::vec3(rot * glm::vec4(0, 0, -1, 0)));
}

glm::vec3 TransformComponent::right() const
{
	glm::mat4 rot = getRotationMatrix();
	return glm::normalize(glm::vec3(rot * glm::vec4(1, 0, 0, 0)));
}

glm::vec3 TransformComponent::up() const
{
	glm::mat4 rot = getRotationMatrix();
	return glm::normalize(glm::vec3(rot * glm::vec4(0, 1, 0, 0)));
}

glm::vec3 TransformComponent::getPosition() const {
	return position;
}

glm::vec3 TransformComponent::getRotation() const {
	return rotation;
}

glm::vec3 TransformComponent::getScale() const {
	return scale;
}