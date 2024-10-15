
// Defines the BlockType enum, this is simply an enumeration of all the block types available in the game.
// Note: All the blocks properties are defined in the WorldEncyclopedia class.
// Note: The integer associated to the BlockType is used as a tile ID into the blocks 

// Note: The block type is actually an uint8_t and is used as a tile id in the blocks tileset so make sure to keep those in sync,
// in other words the order of blocks in the tileset must match the order of declaration in the enum!!! (AIR is not rendered so is not present in the tileset)

#ifndef BLOCK_TYPES_H
#define BLOCK_TYPES_H

#include <cstdint>

namespace mc2d {


        constexpr float BLOCK_WIDTH = 1.0f;
        constexpr float BLOCK_HEIGHT = 1.0f;


        // Note: If you change this enum make sure to update the look-up table in WorldEncyclopedia class too (in worldEncyclopeida.cpp) and keep the same order of elements!
        // Remember to update "resources/myTileset.png" too.
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

                AIR,                    // Keep me here!!!
                BLOCK_TYPE_MAX          // Keep me last!!!
        };

}

#endif // BLOCK_TYPES_H
