
// Defines the BlockType enum, the Block and BlocksTable classes.
//
// The Block class defines all the properties of a certain block type.
//
// The BlocksTable class holds a look up table that enables us to get all the properties
// of a block using only its block type
//
// Note: The block type is actually an uint8_t and is used as an index into the blocks look up table and in the
// blocks tileset so make sure to keep those in sync, in other words the order of blocks in the look up table and in the 
// tileset must match the order of declaration in the enum!!! (AIR is not rendered so is not present in the tileset)
//


#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>

namespace mc2d {

        constexpr float BLOCK_WIDTH = 1.0f;
        constexpr float BLOCK_HEIGHT = 1.0f;

        // Enumeration of all block types available in the game
        enum class BlockType : uint8_t {
                GRASS = 0,
                DIRT,
                STONE,
                COBBLESTONE,
                GRAVEL,
                MICELIUM,
                SAND,
                SANDSTONE_RAW,
                SANDSTONE,
                SANDSTONE_POLISHED,
                GRASS_SNOW,
                SNOW,
                ICE,
                CLAY,
                OBSIDIAN,
                BEDROCK,

                OAK_WOOD,
                OAK_LEAF,
                OAK_PLANK,
                OAK_SAPLING,
                BIRCH_WOOD,
                BIRCH_LEAF,
                BIRCH_PLANK,
                BIRCH_SAPLING,
                JUNGLE_WOOD,
                JUNGLE_LEAF,
                JUNGLE_PLANK,
                JUNGLE_SAPLING,
                SPRUCE_WOOD,
                SPRUCE_LEAF,
                SPRUCE_PLANK,
                SPRUCE_SAPLING,

                // Minerals
                COAL_ORE,
                COAL_BLOCK,
                IRON_ORE,
                IRON_BLOCK,
                GOLD_ORE,
                GOLD_BLOCK,
                DIAMOND_ORE,
                DIAMOND_BLOCK,
                EMERALD_ORE,
                EMERALD_BLOCK,
                REDSTONE_ORE,
                REDSTONE_BLOCK,
                LAPISLAZZUILI_ORE,
                LAPISLAZZUILI_BLOCK,

                // Others
                MOSSY_COBBLESTONE,
                MOSSY_BRICK,

                // Furnitures
                WORKBENCH,
                FURNACE,
                FURNACE_ACTIVE,
                CHEST,
                DOOR_OPEN_LOW,
                DOOR_OPEN_HIGH,
                DOOR_CLOSED,
                TRAP_OPEN,
                TRAP_CLOSED,
                STAIR,
                BED_END,
                BED_START,

                // Others
                TNT,
                WOOL,
                PISTON,
                ENCHANTMENT_BENCH,
                
                BRICK,
                GLASS,
                LIBRARY,
                FLOWER_RED,
                FLOWER_YELLOW,
                SHRUB,
                MUSHROOM_RED,
                MUSHROOM_BROWN,

                // Liquids
                WATER,
                LAVA,

                AIR,            // Keep me here
                BLOCK_TYPE_MAX
        };


        class Block {
        public:
                Block(bool collidable, uint8_t hardness, uint8_t luminance);
                ~Block() = default;

                inline const bool       isCollidable()  const   { return static_cast<bool>(m_properties & 0b01000000); }
                inline const uint8_t    getHardness()   const   { return static_cast<uint8_t>( (m_properties & 0b111000) >> 3); }
                inline const uint8_t    getLuminance()  const   { return static_cast<uint8_t>(m_properties & 0b111); }

        private:
                uint8_t         m_properties;                   // Bit field in which all the block properties are embedded
        };


        class BlocksTable {
        public:
                BlocksTable() = delete;
                ~BlocksTable() = delete;

                static inline const Block&      getBlockDetails(const BlockType type);

        private:
                static const Block              s_blocksLUT[];
        };

}

#endif // BLOCK_H
