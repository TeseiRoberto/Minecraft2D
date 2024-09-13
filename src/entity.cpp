
#include "entity.hpp"
#include "world/gameWorld.hpp"
#include "logging.hpp"

namespace mc2d {

        Entity::Entity(const glm::vec3& pos, float health, EntityType entityType) :
                m_pos(pos), m_velocity(glm::vec3(0.0f)), m_acceleration(glm::vec3(0.0f)),
                m_health(health), m_isFacingRight(true), m_entityType(entityType)
        {}


        // Updates the entity properties
        // @deltaTime: time passed from the last update (measured in milliseconds)
        void Entity::update(float deltaTime)
        {
                // Entity velocity and acceleration are expressed in seconds so we need to convert the delta time
                float deltaTimeInSec = deltaTime / 1000.0f;

                m_velocity += deltaTimeInSec * m_acceleration;
                m_pos += deltaTimeInSec * m_velocity;

                m_velocity *= 0.9f;            // Simulate drag

                m_acceleration.x = 0.0f;
                m_acceleration.y = 0.0f;
                m_acceleration.z = 0.0f;
        }

}

