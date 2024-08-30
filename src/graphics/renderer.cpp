
#include "renderer.hpp"

namespace mc2d {


        Renderer::Renderer() : m_isInit(false), m_spriteVao(0), m_spriteVbo(0)
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
                glFrontFace(GL_CCW);

                if(initSpriteRenderingData() != 0)              // Initialize resources needed to render sprites
                {
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
                glDrawArrays(GL_TRIANGLES, 0, 6);                       // Render textured quad
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
        }

}



