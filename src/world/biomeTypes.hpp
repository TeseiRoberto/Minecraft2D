
// Contains definition of BiomeType enum, this is simply an enumeration of all the biome types available in the game.
// Note: All the biome properties are defined in the WorldEncyclopedia class.

#ifndef BIOME_TYPES_H
#define BIOME_TYPES_H

#include <cstdint>

namespace mc2d {


        // Note: If you change this enum make sure to update the look-up table in WorldEncyclopedia class too (in worldEncyclopeida.cpp) and keep the same order of elements!
        enum class BiomeType : uint32_t {
                PLAIN = 0,              // Mostly flat terrain (made of: grass, dirt, stone)
                FOREST,                 // Hilly terrain (made of: grass, dirt, stone) with oak and birch trees
                MOUNTAIN,               // Pretty steep terrain (made of: grass dirt and stone)
                SNOWY_PLAIN,            // Mostly flat terrain (made of: snowy grass, dirt, stone)
                SNOWY_FOREST,           // Hilly terrain (made of: snowy grass, dirt, stone) with spruce trees
                SNOWY_MOUNTAIN,         // Pretty steep terrain (made of: snowy grass dirt and stone)
                JUNGLE,                 // Hilly terrain (made of: grass, dirt and stone)

                SUPER_FLAT,             // Super flat terrain (made of: grass, dirt and stone)
                BIOME_TYPE_MAX          // Keep me last!!!
        };

}

#endif // BIOME_TYPES_H

