
// Contains definition of the Chunk struct, this struct contains a vector that keeps
// track of all the blocks that makes up a portion (a chunk) of the game world

#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstdint>

#include "block.hpp"
#include "logging.hpp"

namespace mc2d {

        constexpr float BLOCK_WIDTH = 1.0f;
        constexpr float BLOCK_HEIGHT = 1.0f;

        struct Chunk {
                static uint8_t          width;                  // The width of the chunk (in blocks)
                static uint8_t          height;                 // The height of the chunk (in blocks)

                bool                    hasChanged;             // Flag used to indicate that a block has changed
                std::vector<BlockType>  blocks;                 // Keeps track of all the blocks in the chunk

                void                    generateRandom();
                void                    generateFlatChunk();
        };
}

#endif // CHUNK_H
