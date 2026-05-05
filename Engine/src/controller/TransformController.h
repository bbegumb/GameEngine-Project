#pragma once


#include <scene/Entity.h>
#include <glm/glm.hpp>

class TransformController {
public:
    static glm::vec3 getPosition(Entity* entity);
    static glm::vec3 getRotation(Entity* entity);
    static glm::vec3 getScale(Entity* entity);

    static void setPosition(Entity* entity, const glm::vec3& position);
    static void setRotation(Entity* entity, const glm::vec3& rotation);
    static void setScale(Entity* entity, const glm::vec3& scale);
};
