
#include "block.hpp"

namespace mc2d {


        // Block constructor, packs the given properties into a single uint8_t
        // @collidable: indicates if entities can collide with the block
        // @hardness: how difficult it is to break the block (from 0 to 7)
        // @luminance: amount of light emitted by this block (from 0 to 7)
        Block::Block(bool collidable, uint8_t hardness, uint8_t luminance) : m_properties(0)
        {
                // First 3 bits (from right) stores the luminance
                m_properties |= (luminance & 0b111);

                // Next 3 bits stores the hardness
                m_properties |= ( (hardness & 0b111) << 3 );

                // Second most left bit stores the collidable value
                if(collidable)
                        m_properties |= 0b01000000;
        }



        // Returns all the properties associated to the given block type
        const Block& BlocksTable::getBlockDetails(const BlockType type)
        {
                return s_blocksLUT[ static_cast<uint8_t>(type)];
        }


        // Definition of the blocks look up table
        // (If you add something make sure that the order with BlockType enum is respected)
        const Block BlocksTable::s_blocksLUT[] = {
                // GRASS
                Block(true, 2, 0),

                // DIRT
                Block(true, 2, 0),

                // STONE
                Block(true, 4, 0),

                // COBBLESTONE
                Block(true, 4, 0),

                // GRAVEL
                Block(true, 2, 0),

                // MICELIUM
                Block(true, 2, 0),

                // SAND
                Block(true, 2, 0),

                // SANDSTONE_RAW
                Block(true, 4, 0),

                // SANDSTONE
                Block(true, 4, 0),

                // SANDSTONE_POLISHED
                Block(true, 4, 0),

                // GRASS_SNOW
                Block(true, 2, 0),

                // SNOW
                Block(true, 2, 0),

                // ICE
                Block(true, 2, 0),

                // CLAY
                Block(true, 2, 0),

                // OBSIDIAN
                Block(true, 8, 0),

                // BEDROCK
                Block(true, 255, 0),

                // ===========================[ Wood and trees ]=========================== 
                // OAK_WOOD
                Block(true, 3, 0),

                // OAK_LEAF
                Block(true, 2, 0),

                // OAK_PLANK
                Block(true, 3, 0),

                // OAK_SAPLING
                Block(true, 0, 0),

                // BIRCH_WOOD
                Block(true, 3, 0),

                // BIRCH_LEAF
                Block(true, 2, 0),

                // BIRCH_PLANK
                Block(true, 3, 0),

                // BIRCH_SAPLING
                Block(true, 0, 0),

                // JUNGLE_WOOD
                Block(true, 3, 0),

                // JUNGLE_LEAF
                Block(true, 2, 0),

                // JUNGLE_PLANK
                Block(true, 3, 0),

                // JUNGLE_SAPLING
                Block(true, 0, 0),

                // SPRUCE_WOOD
                Block(true, 3, 0),

                // SPRUCE_LEAF
                Block(true, 2, 0),

                // SPRUCE_PLANK
                Block(true, 3, 0),

                // SPRUCE_SAPLING
                Block(true, 0, 0),

                // ===========================[ Minerals ]=========================== 
                // COAL_ORE
                Block(true, 4, 0),

                // COAL_BLOCK
                Block(true, 4, 0),

                // IRON_ORE
                Block(true, 4, 0),

                // IRON_BLOCK
                Block(true, 4, 0),

                // GOLD_ORE
                Block(true, 4, 0),

                // GOLD_BLOCK
                Block(true, 4, 0),

                // DIAMOND_ORE
                Block(true, 4, 0),

                // DIAMOND_BLOCK
                Block(true, 4, 0),

                // EMERALD_ORE
                Block(true, 4, 0),

                // EMERALD_BLOCK
                Block(true, 4, 0),

                // REDSTONE_ORE
                Block(true, 4, 0),

                // REDSTONE_BLOCK
                Block(true, 4, 0),

                // LAPISLAZZULI_ORE
                Block(true, 4, 0),

                // LAPISLAZZULI_BLOCK
                Block(true, 4, 0),

                // ===========================[ Furnitures ]=========================== 
                // WORKBENCH
                Block(true, 2, 0),

                // FURNACE
                Block(true, 3, 0),

                // FURNACE_ACTIVE
                Block(true, 3, 0),

                // CHEST
                Block(true, 2, 0),

                // DOOR_OPEN_LOW
                Block(true, 2, 0),

                // DOOR_OPEN_HIGH
                Block(true, 2, 0),

                // DOOR_CLOSED
                Block(true, 2, 0),

                // TRAP_OPEN
                Block(true, 2, 0),

                // TRAP_CLOSED
                Block(true, 2, 0),

                // STAIR
                Block(true, 2, 0),

                // BED_END
                Block(true, 2, 0),

                // BED_START
                Block(true, 2, 0),

                // ===========================[ Others ]=========================== 
                // TNT
                Block(true, 0, 0),

                // WOOL
                Block(true, 2, 0),

                // PISTON
                Block(true, 3, 0),

                // ENCHANTMENT_BENCH
                Block(true, 3, 0),

                // BRICK
                Block(true, 4, 0),

                // GLASS
                Block(true, 2, 0),

                // LIBRARY
                Block(true, 3, 0),

                // FLOWER_RED
                Block(true, 0, 0),

                // FLOWER_YELLOW
                Block(true, 0, 0),

                // SHRUB
                Block(true, 0, 0),

                // MUSHROOM_RED
                Block(true, 0, 0),

                // MUSHROOM_BROWN
                Block(true, 0, 0),

                // ===========================[ Liquids ]=========================== 
                // Water
                Block(true, 0, 0),

                // Lava
                Block(true, 0, 0),

                // AIR (Keep me last)
                Block(false, 0, 0)
        };

}
