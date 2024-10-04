
// Contains definition of the Entity class

#ifndef ENTITY_H
#define ENTITY_H

#include <fstream>
#include <glm/vec3.hpp>

namespace mc2d {


        enum class EntityType {
                PLAYER,
                CHICKEN,

                // TODO: Add other entities
        };


        class Entity {
        public:
                Entity(const glm::vec3& pos, float health, EntityType entityType);
                ~Entity() = default;

                void                    update(float deltaTime);

                inline void             setPos(const glm::vec3& pos)            { m_pos = pos; }
                inline void             setVelocity(const glm::vec3& vel)       { m_velocity = vel; }
                inline void             setAcceleration(const glm::vec3& acc)   { m_acceleration = acc; }
                inline void             setHealth(float health)                 { m_health = health; }
                inline void             setIsFacingRight(bool value)            { m_isFacingRight = value; }

                inline const glm::vec3& getPos() const                          { return m_pos; }
                inline const glm::vec3& getVelocity()                           { return m_velocity; }
                inline const glm::vec3& getAcceleration()                       { return m_acceleration; }
                inline float            getHealth() const                       { return m_health; }
                inline const EntityType getType() const                         { return m_entityType; }
                inline const bool       isFacingRight() const                   { return m_isFacingRight; }

                inline void             updatePos(float x, float y)             { m_pos.x += x; m_pos.y += y; }

                bool                    serialize(std::ofstream& file) const;
                bool                    deserialize(std::ifstream& file);


        private:
                glm::vec3       m_pos;                  // The position of the entity in world coordinates
                glm::vec3       m_velocity;             // Velocity with whom the entity is moving (measured in blocks/second)
                glm::vec3       m_acceleration;         // Acceleration with whom the entity is moving (measured in blocks/second^2)

                float           m_health;
                bool            m_isFacingRight;
                EntityType      m_entityType;
        };

}

#endif // ENTITY_H
