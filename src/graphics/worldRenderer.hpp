
// Contains definition of the WorldRenderer class.
// The WorldRenderer implements batch rendering to draw all the blocks (that makes up a world)
// that are visible from a camera.
//

#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include <cstdint>
#include <glad/glad.h>

#include "logging.hpp"
#include "world/gameWorld.hpp"
#include "shader.hpp"
#include "tileset.hpp"
#include "sprite.hpp"
#include "camera.hpp"

namespace mc2d {


        class WorldRenderer {
        public:
                WorldRenderer();
                ~WorldRenderer();

                // Delete copy constructors
                WorldRenderer(WorldRenderer& other) = delete;
                WorldRenderer(const WorldRenderer& other) = delete;
                WorldRenderer operator = (WorldRenderer& other) = delete;
                WorldRenderer operator = (const WorldRenderer& other) = delete;

                int             init(size_t maxBlocksInBatch);
                void            terminate();
                inline bool     isInit() const                                  { return m_isInit; }

                void            render(GameWorld& world, Camera& camera, bool optimized);

        private:

                // This struct defines the memory layout of one vertex that makes up a block,
                // each block is made up of 6 of those (2 triangles with 3 vertices each)
                struct BlockVertex {
                        float   position[2];
                        float   uv[2];
                        float   tileId;
                };

                size_t          computeVisibleBlocksVertices(const GameWorld& world, const Camera& camera, BlockVertex* vertices, const size_t maxVerticesNum, size_t& verticesNum);
                size_t          optimizedComputeVisibleBlocksVertices(const GameWorld& world, const Camera& camera, BlockVertex* vertices, const size_t maxVerticesNum, size_t& verticesNum);

                bool            generateBlockVertices(BlockVertex* vertices, size_t& index, const size_t& maxVerticesNum,
                                                const float& startX, const float& startY, const float& endX, const float& endY, BlockType block) const;

                bool            m_isInit;
                Tileset         m_blocksTileset;                // Tileset that contains the blocks textures

                size_t          m_maxBlocksInBatch;             // Maximum number of blocks that can be rendered in a single draw call
                size_t          m_currBlocksInBatch;            // Number of blocks that will be rendered in the next draw call
                BlockVertex*    m_blocksVertices;               // Vertices data for the blocks that will be rendered in the next draw call
                
                uint32_t        m_worldVao;
                uint32_t        m_worldVbo;
                Shader          m_worldShader;
        };

}

#endif // WORLD_RENDERER_H
