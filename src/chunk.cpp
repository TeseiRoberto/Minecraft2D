
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

}
