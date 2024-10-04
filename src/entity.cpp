
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


        // Writes the entity data in the given file
        // @file: output file stream in which entity data will be written
        // @returns: true if serialization is successfull, false otherwise
        bool Entity::serialize(std::ofstream& file) const
        {
                if(!file.good())
                {
                        logError("Entity::serialize() failed, the given file stream is broken");
                        return false;
                }

                file << m_pos.x << ' ' << m_pos.y << ' ' << m_pos.z << ' ';
                file << m_velocity.x << ' ' << m_velocity.y << ' ' << m_velocity.z << ' ';
                file << m_health << ' ';
                file << m_isFacingRight << ' ';
                file << static_cast<uint32_t>(m_entityType) << ' ';
                file << '\n';

                return file.good();
        }


        // Reads entity data from the given file
        // @file: input file stream from which entity data will be read
        // @returns: true if deserialization is successfull, false otherwise
        bool Entity::deserialize(std::ifstream& file)
        {
                if(!file.good())
                {
                        logError("Entity::deserialize() failed, the given file stream is broken");
                        return false;
                }

                glm::vec3 pos(0.0f);
                glm::vec3 velocity(0.0f);
                float health;
                bool isFacingRight;
                uint32_t entityType;

                file >> pos.x; file >> pos.y; file >> pos.z;
                file >> velocity.x; file >> velocity.y; file >> velocity.z;
                file >> health;
                file >> isFacingRight;
                file >> entityType;

                // If data serialization was successfull then we can use such data to setup this entity
                if(file.good())
                {
                        m_pos = pos;
                        m_velocity = velocity;
                        m_health = health;
                        m_isFacingRight = isFacingRight;
                        m_entityType = static_cast<EntityType>(entityType);
                        return true;
                }

                logError("Entity::deserialize(), cannot deserialize entity's data");
                return false;
        }
}

