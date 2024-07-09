
// Defines the Block struct and the blocksTable global variable

#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>

namespace mc2d {

        using BlockType = uint8_t;

        namespace Block {

                constexpr BlockType AIR = 0;

        }


        // TODO: For now I don't really need a Block struct/class so I have not defined it yet, I will in the future...

        // Look up table that enables us to find all the properties of a block using only it's block type
        //const Block   blocksTable[];
}

#endif // BLOCK_H
