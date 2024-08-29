
// Contains definition of the GameWorld class and the Chunk struct.
//
// The GameWorld class has responsibility of managing chunks (loading, unloading, ...).
//
// The Chunk struct is used to keep track of all the blocks that makes up a portion of the game world and all the entities
// that are contained in it.
//
// The game world is structured as a sequence of chunks, the chunk in which the player spawn is the root chunk and
// has the id 0; chunks to the left of the root chunk have negative ids while chunks to the right have positive ids.
// The coordinate system used in the game world is a cartesian system which has the origin positioned in the bottom left
// block of the root chunk, in particular the position (0,0) in world space corresponds to the bottom left vertex of such block.
//

#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include <glm/vec2.hpp>

#include "logging.hpp"
#include "block.hpp"
#include "entity.hpp"

namespace mc2d {

        enum class BiomeType : uint32_t;
        class WorldGenerator;

        struct Chunk {
                static constexpr uint8_t width = 18;            // Width of the chunk measured in blocks, never set below 8!
                static constexpr uint8_t height = 18;           // Height of the chunk measured in blocks, never set below 8!

                int                     id;                     // Uniquely identifies the chunk in the game world (is negative for left chunks, positive for the right ones)
                BiomeType               biome;
                std::vector<BlockType>  blocks;                 // Keeps track of all the blocks in the chunk
                std::vector<Entity>     entities;               // Keeps track of all the entities that are contained in the chunk

                // Returns the coordinates (in world space) of the top left corner of the chunk
                inline glm::vec2  getPos() const                { return glm::vec2( (float) (id * Chunk::width) * BLOCK_WIDTH, (float) Chunk::height * BLOCK_HEIGHT); }
        };


        class GameWorld {
        public:
                friend class WorldGenerator;

                GameWorld();
                GameWorld(std::vector<Chunk>&& chunks);
                ~GameWorld() = default;

                GameWorld&                              operator = (GameWorld&& world);

                bool                                    loadFromFile(const std::string& filepath);
                bool                                    saveToFile(const std::string& filepath);

                void                                    update(float deltaTime);

                void                                    setBlock(float x, float y, BlockType newBlock);
                inline void                             setHasChanged(bool changed)                     { m_hasChanged = changed; }

                BlockType                               getBlock(float x, float y) const;
                inline bool                             hasChanged() const                              { return m_hasChanged; }

                void                                    appendChunk(Chunk&& newChunk);
                void                                    removeChunk(int id);
                inline const std::vector<Chunk>&        getLoadedChunks() const                         { return m_loadedChunks; }

                inline Entity&                          getPlayer()                                     { return m_player; }
                Chunk*                                  getPlayerChunk();


        private:
                void                    initializeDummyWorld();

                bool                    m_hasChanged;           // Flag used to indicate that one or more blocks have been added/removed from the world
                std::vector<Chunk>      m_loadedChunks;
        
                Entity                  m_player;               // The main player
        };

}

#endif // GAME_WORLD_H

