
// Contains definition of the GameWorld class and the Chunk struct.
//
// The GameWorld class has responsibility of managing chunks (loading, unloading, ...).
// The Chunk struct is used to keep track of all the blocks that makes up a portion of the game world.
//
// The game world is structured as a sequence of chunks, the chunk in which the player spawns is the root chunk and
// has the id 0; chunks to the left of the root chunk have negative ids while chunks to the right have positive ids.
// The coordinate system used in the game world has the origin positioned in the bottom left block of the root chunk,
// in particular the position (0,0) in world space corresponds to the bottom left vertex of such block.
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
#include "worldGenerator.hpp"


namespace mc2d {


        struct Chunk {
                static uint8_t          width;
                static uint8_t          height;

                int                     id;                     // Uniquely identifies the chunk in the game world (is negative for left chunks, positive for the right ones)
                std::vector<BlockType>  blocks;                 // Keeps track of all the blocks in the chunk


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

                void                                    setBlock(float x, float y, BlockType newBlock);
                inline void                             setHasChanged(bool changed)                     { m_hasChanged = changed; }

                BlockType                               getBlock(float x, float y) const;
                inline bool                             hasChanged() const                              { return m_hasChanged; }

                void                                    appendChunk(Chunk&& newChunk);
                void                                    removeChunk(int id);
                inline const std::vector<Chunk>&        getLoadedChunks() const                               { return m_loadedChunks; }
                inline const Chunk&                     getPlayerChunk() const                          { return *m_playerChunk; }


        private:
                void                    initializeDummyWorld();

                bool                    m_hasChanged;           // Flag used to indicate that one or more blocks have been added/removed from the world
                Chunk*                  m_playerChunk;          // Pointer to the chunk in which the player is (must never be nullptr)
                std::vector<Chunk>      m_loadedChunks;
        };

}

#endif // GAME_WORLD_H

