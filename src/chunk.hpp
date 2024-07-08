
// Contains definition of the Chunk struct, this struct contains a vector that keeps
// track of all the blocks that makes up a portion (a chunk) of the game world

#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdint>

#include "logging.hpp"

namespace mc2d {

        struct Chunk {
                static uint8_t          width;                  // The width of the chunk (in blocks)
                static uint8_t          height;                 // The height of the chunk (in blocks)

                bool                    hasChanged;             // Flag used to indicate that a block has changed
                std::vector<bool>       blocks;                 // Keeps track of all the blocks in the chunk

                void                    generate();
        };
}

#endif // CHUNK_H
