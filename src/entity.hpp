
// Contains definition of the Entity class

#ifndef ENTITY_H
#define ENTITY_H

#include <glm/vec3.hpp>

namespace mc2d {


        enum class EntityType {
                PLAYER,
                CHICKEN,
                ZOMBIE

                // TODO: Add other entities
        };


        class Entity {
        public:
                Entity(const glm::vec3& pos, float health, EntityType entityType);
                ~Entity() = default;

                inline void             setPos(const glm::vec3& pos)    { m_pos = pos; }
                inline void             setHealth(float health)         { m_health = health; }

                inline glm::vec3        getPos() const                  { return m_pos; }
                inline float            getHealth() const               { return m_health; }
                inline EntityType       getType() const                 { return m_entityType; }

                inline void             updatePos(float x, float y)     { m_pos.x += x; m_pos.y += y; }

        private:
                glm::vec3       m_pos;          // The position of the entity in world coordinates
                float           m_health;
                EntityType      m_entityType;
        };

}

#endif // ENTITY_H
