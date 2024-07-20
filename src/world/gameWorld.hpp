
// Contains definition of the GameWorld class and the Chunk struct.
//
// The GameWorld class has responsibility of managing chunks (loading, unloading, generation, ...).
// The Chunk struct is used to keep track of all the blocks that makes up a portion of the game world.
//
// The game world is structured as a sequence of chunks, the chunk in which the player spawns is the root chunk and
// has the id 0; chunks to the left of the root chunk have negative ids while chunks to the right have positive ids.
// The coordinate system used in the game world has the origin positioned in the bottom left block of the root chunk
//

#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <vector>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstdint>

#include "logging.hpp"
#include "block.hpp"
#include "graphics/camera.hpp"

namespace mc2d {


        struct Chunk {
                static uint8_t          width;
                static uint8_t          height;

                int                     id;                     // Uniquely identifies the chunk in the game world (is negative for left chunks, positive for the right ones)
                std::vector<BlockType>  blocks;                 // Keeps track of all the blocks in the chunk
        

                // Returns the x coordinate of the top left corner of the chunk in world space
                inline const float      getXPos() const { return static_cast<float>(id * Chunk::width); }
                
                // Returns the y coordinate of the top left corner of the chunk in world space
                inline const float      getYPos() const { return static_cast<float>(Chunk::height); }
        };


        class GameWorld {
        public:
                GameWorld(uint32_t cachedChunksNum = 3);
                ~GameWorld() = default;

                void                    setBlock(float x, float y, BlockType newBlock);
                inline void             setHasChanged(bool changed)                     { m_hasChanged = changed; }

                BlockType               getBlock(float x, float y) const;
                inline bool             hasChanged() const                              { return m_hasChanged; }
                inline const Chunk&     getPlayerChunk() const                          { return *m_playerChunk; }

                //void                    computeVisibleBlocksVertices(const Camera& camera, float* vertices, size_t maxVerticesNum, size_t* verticesNum);
                //void                    optimizedComputeVisibleBlocksVertices(const Camera& camera, float* vertices, size_t maxVerticesNum, size_t* verticesNum);

                // World generation methods (TODO: move somewhere else)
                void                    generateRandomWorld();
                void                    generateFlatWorld();

        private:
                std::vector<const Chunk*> computeVisibleChunks(const Camera& camera) const;

                void                    generateRandomChunk(Chunk& c);
                void                    generateFlatChunk(Chunk& c);

                bool                    m_hasChanged;           // Flag used to indicate that one or more blocks have been added/removed from the world
                Chunk*                  m_playerChunk;          // Pointer to the chunk in which the player is (must never be nullptr)
                std::vector<Chunk>      m_loadedChunks;
        };

}

#endif // GAME_WORLD_H

