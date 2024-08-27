
#include "camera.hpp"

namespace mc2d {


        // Camera constructor
        // @xPos: x coordinate of the top left corner of the camera in world space
        // @yPos: y coordinate of the top left corner of the camera in world space
        // @zoom: zoom value
        // @width: the width of the camera measured in blocks
        // @height: the height of the camera measured in blocks
        Camera::Camera(float xPos, float yPos, float zoom, uint8_t width, uint8_t height)
                : m_pos( glm::vec3(xPos, yPos, 0.0f) ), m_zoom(zoom), m_width(width), m_height(height), m_hasChanged(true)
        {}


        // Updates the camera width
        // @width: the new width of the camera (must be in range [MIN_WIDTH, UINT8_MAX])
        void Camera::setWidth(uint8_t width)
        {
                if(width < Camera::MIN_WIDTH)
                {
                        logWarn("Camera::setWidth() failed, camera width must be a value between %u and %u!", Camera::MIN_WIDTH, UINT8_MAX);
                        return;
                }

                m_width = width;
                m_hasChanged = true;
        }


        // Updates the camera height
        // @height: the new height of the camera (must be in range [MIN_HEIGHT, UINT8_MAX])
        void Camera::setHeight(uint8_t height)
        {
                if(height < Camera::MIN_HEIGHT)
                {
                        logWarn("Camera::setHeight() failed, camera height must be a value between %u and %u!", Camera::MIN_HEIGHT, UINT8_MAX);
                        return;
                }

                m_height = height;
                m_hasChanged = true;
        }


        // Converts coordinates from window space to world space
        // @x: x coordinate in window space
        // @y: y coordinate in window space
        // @windowWidth: the width of the window 
        // @windowHeight: the height of the window 
        glm::vec2 Camera::windowToWorldCoord(float x, float y, float windowWidth, float windowHeight) const
        {
                float xRelativeToCam = (x / windowWidth) *  (float) m_width;
                float yRelativeToCam = (y / windowHeight) * (float) m_height;

                return glm::vec2(
                        m_pos.x + xRelativeToCam,
                        m_pos.y - yRelativeToCam
                );
        }


        // Returns a matrix that transforms world coordinates to view coordinates
        glm::mat4 Camera::getViewMatrix() const
        {
                return glm::translate(glm::mat4(1.0f), glm::vec3( -m_pos.x, -(m_pos.y - (float) m_height), 0.0f) );
        }


        // Utility function used to determine all the chunks of the given world that are visible from this camera
        // @world: the world that contains the chunks
        std::vector<Chunk const*> Camera::getVisibleChunks(const GameWorld& world) const
        {
                std::vector<const Chunk*> intersectedChunks;
                for(auto& chunk : world.getLoadedChunks())
                {
                        if(doesRectsIntersect(m_pos.x, m_pos.y, (float) m_width, (float) m_height,
                                                chunk.getPos().x, chunk.getPos().y, (float) Chunk::width, (float) Chunk::height))
                                intersectedChunks.push_back(&chunk);
                }

                return intersectedChunks;
        }

}
