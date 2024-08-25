
#include "renderer.hpp"

namespace mc2d {


        Renderer::Renderer() : m_isInit(false),
                m_worldVao(0), m_worldVbo(0), m_worldVerticesBufferSize(Chunk::width * Chunk::height * 30), m_worldVertices(nullptr),
                m_spriteVerticesNum(0), m_spriteVao(0), m_spriteVbo(0)
        {}


        // Terminates the renderer (if not done yet)
        Renderer::~Renderer()
        {
                terminate();
        }


        // Attempts to initialize all the resources needed by the renderer
        // @returns: zero on success, non zero on failure
        int Renderer::init()
        {
                if(m_isInit)
                {
                        logWarn("Renderer::init() failed, renderer has already been initialized!");
                        return 1;
                }

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);           // Set clear color used to clear screen

                if(initWorldRenderingData() != 0)               // Initialize resources needed to render the blocks of the game world
                {
                        logError("Render::init() failed, initWorldRenderingData() failed!");
                        return 1;
                }
               
                if(initSpriteRenderingData() != 0)              // Initialize resources needed to render sprites
                {
                        terminateWorldRenderingData();
                        logError("Render::init() failed, initSpriteRenderingData() failed!");
                        return 1;
                }

                m_isInit = true;
                return 0;
        }


        // Terminates all the resources initialized in the init method
        void Renderer::terminate()
        {
                if(!m_isInit)
                        return;

                terminateWorldRenderingData();
                terminateSpriteRenderingData();

                m_isInit = false;
        }


        // Resizes the viewport
        // @newWidth: the new width of the viewport
        // @newHeight: the new height of the viewport
        void Renderer::resizeViewport(int newWidth, int newHeight)
        {
                if(!m_isInit)
                        return;

                glViewport(0, 0, newWidth, newHeight);
        }


        // Clears the screen with black color
        void Renderer::clearScreen()
        {
                if(!m_isInit)
                        return;

                glClear(GL_COLOR_BUFFER_BIT);
        }


        // Renders all the blocks in the given game world that are visible from the given camera
        void Renderer::renderWorld(GameWorld& world, Camera& camera, bool optimized)
        {
                if(!m_isInit)
                {
                        logWarn("Renderer::renderWorld() failed, renderer has not been initialized correctly!");
                        return;
                }

                m_worldShader.activate();                                               // Activate shader to render the world
                m_blocksTileset.activate();                                             // Bind tileset's texture
                
                // Compute view-projection matrix and set uniform
                glm::mat4 vpMatrix = glm::ortho(0.0f, (float) camera.getWidth(), 0.0f, (float) camera.getHeight()) * camera.getViewMatrix();
                int viewMatUniform = m_worldShader.getUniformId("transformMatrix");
                m_worldShader.setUniform(viewMatUniform, vpMatrix);

                glBindVertexArray(m_worldVao);                                          // Bind world vao

                // If chunk has changed then we must recalculate the vertices of all the blocks in the game world
                // that are visible and we need to update data in the world vbo
                if(world.hasChanged() || camera.hasChanged())
                {
                        if(optimized)
                                camera.optimizedComputeVisibleBlocksVertices(world, m_worldVertices, m_worldVerticesBufferSize, &m_worldVerticesNum);
                        else
                                camera.computeVisibleBlocksVertices(world, m_worldVertices, m_worldVerticesBufferSize, &m_worldVerticesNum);

                        glBindBuffer(GL_ARRAY_BUFFER, m_worldVbo);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, m_worldVerticesNum * sizeof(float), m_worldVertices);

                        world.setHasChanged(false);
                        camera.setHasChanged(false);
                }

                glDrawArrays(GL_TRIANGLES, 0, m_worldVerticesNum);
        }


        // Renders the given sprite
        // @sprite: the 2D image to be rendered
        // @pos: defines the position (in world space coordinates) of the top left corner of the sprite
        // @scale: defines the scale factor of the sprites relative to the x, y and z axis
        // @rotation: the angle of rotation (in degrees) of the sprite relative to the x axis
        // @camera: used to determine the view matrix
        void Renderer::renderSprite(const Sprite& sprite, const glm::vec3& pos, const glm::vec3& scale, const float rotation, const Camera& camera)
        {
                // Compute model matrix for the sprite
                glm::mat4 id(1.0f);
                glm::mat4 modelMat = glm::translate(id, pos);
                modelMat *= glm::rotate(id, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
                modelMat *= glm::scale(id, scale);

                return renderSprite( sprite, modelMat, camera.getViewMatrix(), glm::ortho(0.0f, (float) camera.getWidth(), 0.0f, (float) camera.getHeight()) );
        }


        // Renders the given sprite
        // @sprite: the 2D image to be rendered
        // @modelMatrix: model matrix used to transform the sprite
        // @viewMatrix: view matrix
        // @projectionMatrix: projection matrix used to project the transformed sprite onto the screen
        void Renderer::renderSprite(const Sprite& sprite, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
        {
                if(!m_isInit)
                {
                        logWarn("Renderer::renderSprite() failed, renderer has not been initialized correctly!");
                        return;
                }

                m_spriteShader.activate();
                sprite.activate();

                // Compute view-projection matrix and set uniform
                glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
                int mvpMatUniform = m_spriteShader.getUniformId("mvpMatrix");
                m_spriteShader.setUniform(mvpMatUniform, mvpMatrix);

                glBindVertexArray(m_spriteVao);                         // Bind vao to render sprite quad
                glDrawArrays(GL_TRIANGLES, 0, m_spriteVerticesNum);     // Render sprite
        }


        // Initializes all the resources needed to render the bloks of the game world
        int Renderer::initWorldRenderingData()
        {
                // Load the tileset that contains all the game's blocks
                if(!m_blocksTileset.load("../resources/textures/myTileset.png", 5, 16, 16, 16))
                {
                        logError("Renderer::initWorldRenderingData() failed, game tileset creation failed!");
                        return 1;
                }

                // Initialize shader, vao and vbo needed to render the blocks in the game world
                if(m_worldShader.init("../resources/worldVrtxShader.vert", "../resources/worldFragShader.frag") != 0)
                {
                        logError("Renderer::initWorldRenderingData() failed, world shader creation failed!");
                        m_blocksTileset.unload();
                        return 1;
                }

                // Generate vao and vbo to render world's blocks
                glGenVertexArrays(1, &m_worldVao);
                glBindVertexArray(m_worldVao);

                glGenBuffers(1, &m_worldVbo);
                glBindBuffer(GL_ARRAY_BUFFER, m_worldVbo);

                // Allocate space for world vbo
                glBufferData(GL_ARRAY_BUFFER, Chunk::width * Chunk::height * 30 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

                // Define format of data in world vbo (so the vertex shader knows how to use such data)
                // Vertex's X and y coordinates
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
                glEnableVertexAttribArray(0);

                // Vertex's UV coordinates
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (2 * sizeof(float)) );
                glEnableVertexAttribArray(1);

                // Vertex's tile id (id of the texture in the tileset's texture array)
                glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (4 * sizeof(float)) );
                glEnableVertexAttribArray(2);

                // Allocate a memory buffer to compute world data
                m_worldVerticesNum = 0;
                m_worldVertices = new float[m_worldVerticesBufferSize];
                
                return 0;
        }


        // Terminates and deallocates all the resources needed for rendering the blocks of the game world
        void Renderer::terminateWorldRenderingData()
        {
                if(m_worldShader.isInit())
                {
                        m_worldShader.deactivate();
                        m_worldShader.terminate();
                }

                if(m_blocksTileset.isInit())
                {
                        m_blocksTileset.deactivate();
                        m_blocksTileset.unload();
                }

                if(m_worldVao != 0)
                {
                        glDeleteVertexArrays(1, &m_worldVao);
                        m_worldVao = 0;

                        glDeleteBuffers(1, &m_worldVbo);
                        m_worldVbo = 0;
                }

                m_worldVerticesNum = 0;
                delete[] m_worldVertices;
        }


        // Initializes all the resources needed to render sprites
        int Renderer::initSpriteRenderingData()
        {
                if(m_spriteShader.init("../resources/spriteVrtxShader.vert", "../resources/spriteFragShader.frag") != 0)
                {
                        logError("Render::initSpriteRenderingData() failed, sprite shader creation failed!");
                        return 1;
                }

                float quadVertices[] = {
                //       x     y         u     v
                        0.0f, 0.0f,     0.0f, 0.0f,     // Bottom left
                        1.0f, 0.0f,     1.0f, 0.0f,     // Bottom right
                        0.0f, 1.0f,     0.0f, 1.0f,     // Top left
                        1.0f, 0.0f,     1.0f, 0.0f,     // Bottom right
                        1.0f, 1.0f,     1.0f, 1.0f,     // Top right
                        0.0f, 1.0f,     0.0f, 1.0f,     // Top left
                };

                m_spriteVerticesNum = sizeof(quadVertices) / sizeof(float);

                // Generate vao and vbo for sprite rendering
                glGenVertexArrays(1, &m_spriteVao);
                glBindVertexArray(m_spriteVao);

                glGenBuffers(1, &m_spriteVbo);
                glBindBuffer(GL_ARRAY_BUFFER, m_spriteVbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

                // Define format of data in sprite vbo
                // Vertex's X and y coordinates
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
                glEnableVertexAttribArray(0);

                // Vertex's UV coordinates
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)) );
                glEnableVertexAttribArray(1);

                return 0;
        }


        // Terminates all the resources needed to render sprites
        void Renderer::terminateSpriteRenderingData()
        {
                if(m_spriteShader.isInit())
                {
                        m_spriteShader.deactivate();
                        m_spriteShader.terminate();
                }

                if(m_spriteVao != 0)
                {
                        glDeleteVertexArrays(1, &m_spriteVao);
                        m_spriteVao = 0;

                        glDeleteBuffers(1, &m_spriteVbo);
                        m_spriteVbo = 0;
                }

                m_spriteVerticesNum = 0;
        }

}



