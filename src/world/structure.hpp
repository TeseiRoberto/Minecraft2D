
// Contains definition of the Structure class.
// This class is used to represent a structure that needs to be spawned in the game world.
//

#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <vector>
#include <fstream>
#include <glm/vec2.hpp>

#include "log.hpp"
#include "blockTypes.hpp"

namespace mc2d {


        class Structure {
        public:
                Structure();
                Structure(glm::vec2 spawnPoint, std::vector<BlockType>& blocks, size_t width, size_t height, glm::vec2 origin);
                ~Structure() = default;

                inline const glm::vec2                  getSpawnPoint() const   { return m_spawnPoint; }
                inline const size_t                     getWidth() const        { return m_width; }
                inline const size_t                     getHeight() const       { return m_height; }
                inline const std::vector<BlockType>&    getBlocks() const       { return m_blocks; }
                inline const glm::vec2                  getOrigin() const       { return m_origin; }

                bool                                    serialize(std::ofstream& file) const;
                bool                                    deserialize(std::ifstream& file);

        private:
                glm::vec2               m_spawnPoint;   // Coordinates at which the origin of the structure must be placed
                size_t                  m_width;        // The width of the structure
                size_t                  m_height;       // The height of the structure
                std::vector<BlockType>  m_blocks;       // Blocks that makes up the structure
                glm::vec2               m_origin;       // Indexes relative to m_blocks that defines the block that will be placed at the spawn point
        };
}

#endif // STRUCTURE_H
