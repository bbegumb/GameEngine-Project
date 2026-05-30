#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>

TransformComponent::~TransformComponent() {
	if (parent)
		parent->removeChild(this);

	for (auto* child : children)
		child->parent = nullptr;
}

glm::mat4 TransformComponent::getMatrix() const {
	if (isMatrixValid) return modelMatrix;

	glm::mat4 local = glm::mat4(1.0f);
	local = glm::translate(local, position);
	local *= glm::mat4_cast(rotation);
	local = glm::scale(local, scale);

	modelMatrix = parent ? parent->getMatrix() * local : local;
	isMatrixValid = true;
	return modelMatrix;
}

void TransformComponent::invalidate() {
	isMatrixValid = false;
	for (auto* child : children)
		child->invalidate();
}

void TransformComponent::setParent(TransformComponent* newParent) {
	if (onBeforeReparent && !onBeforeReparent()) return;

	if (parent == newParent) return;

	glm::mat4 worldMatrix = getMatrix();

	if (parent)
		parent->removeChild(this);
	parent = newParent;
	if (parent)
		parent->children.push_back(this);

	if (parent) {
		glm::mat4 localMatrix = glm::inverse(parent->getMatrix()) * worldMatrix;

		position = glm::vec3(localMatrix[3]);
		scale = glm::vec3(
			glm::length(glm::vec3(localMatrix[0])),
			glm::length(glm::vec3(localMatrix[1])),
			glm::length(glm::vec3(localMatrix[2])));

		glm::mat3 rotMat(
			glm::vec3(localMatrix[0]) / scale.x,
			glm::vec3(localMatrix[1]) / scale.y,
			glm::vec3(localMatrix[2]) / scale.z);
		rotation = glm::quat_cast(rotMat);
	}

	invalidate();
}

void TransformComponent::removeChild(TransformComponent* child) {
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
	invalidate();
}

void TransformComponent::translate(const glm::vec3& delta) {
	position += delta;
	invalidate();
}

void TransformComponent::setPosition(const glm::vec3& pos) {
	position = pos;
	invalidate();
}

void TransformComponent::setRotation(const glm::vec3& eulerAngles) {
	rotation = glm::quat(eulerAngles);
	invalidate();
}

void TransformComponent::setRotation(const glm::quat& quat) {
	rotation = quat;
	invalidate();
}

void TransformComponent::rotate(const glm::vec3& eulerDelta) {
	glm::quat delta(eulerDelta);
	rotation = delta * rotation;
	invalidate();
}

void TransformComponent::rotateAroundAxis(const glm::vec3& axis, float angle) {
	rotation = glm::angleAxis(angle, glm::normalize(axis)) * rotation;
	invalidate();
}

void TransformComponent::setScale(const glm::vec3& scale) {
	this->scale = scale;
	invalidate();
}

void TransformComponent::scaleBy(const glm::vec3& factor) {
	scale *= factor;
	invalidate();
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