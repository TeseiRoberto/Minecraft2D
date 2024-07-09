
#include "chunk.hpp"

namespace mc2d {

        uint8_t Chunk::width = 32;
        uint8_t Chunk::height = 32;


        // Generates a completely random chunk
        void Chunk::generate()
        {
                blocks.reserve(Chunk::width * Chunk::height);
                std::srand(std::time(nullptr));

                for(uint32_t y = 0; y < Chunk::height; y++)
                {
                        for(uint32_t x = 0; x < Chunk::width; x++)
                        {
                                if(std::rand() % 2 == 0)
                                {
                                        blocks[(y * Chunk::width) + x] = 0;
                                } else {
                                        blocks[(y * Chunk::width) + x] = 1;
                                }
                        }
                }

                hasChanged = true;
        }


        // Generates a flat chunk that contains only grass, dirt and stone 
        void Chunk::generateFlatChunk()
        {
                blocks.reserve(Chunk::width * Chunk::height);

                uint32_t offset = 0;

                // Half of the chunk must be filled with air blocks
                std::memset(blocks.data(), 0, (Chunk::height / 2) * Chunk::width);
                offset += ((Chunk::height / 2) * Chunk::width);

                // then we add a layer of grass
                std::memset(blocks.data() + offset, 1, Chunk::width);
                offset += Chunk::width;

                // then we add two layers of dirt
                std::memset(blocks.data() + offset, 2, Chunk::width * 2);
                offset += Chunk::width * 2;

                // and then we add stone blocks until the end of the chunk
                std::memset(blocks.data() + offset, 3, ((Chunk::height / 2) - 3)  * Chunk::width );
                hasChanged = true;
        }
}


