
#include "camera.hpp"

namespace mc2d {

        Camera::Camera(float xPos, float yPos, float zoom, uint8_t width, uint8_t height)
                : m_pos( glm::vec3(xPos, yPos, 0) ), m_zoom(zoom), m_width(width), m_height(height), m_hasChanged(true)
        {}


        // Returns a matrix that transforms world coordinates to view coordinates
        glm::mat4 Camera::getViewMatrix() const
        {
                return glm::translate(glm::mat4(1.0f), -m_pos);
        }

}
