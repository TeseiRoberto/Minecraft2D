
#include "entity.hpp"

namespace mc2d {

        Entity::Entity(const glm::vec3& pos, float health, EntityType entityType) : m_pos(pos), m_health(health), m_entityType(entityType)
        {}

}

