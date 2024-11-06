
// Contains definition of the Camera class, the camera is simply a rectangle which has
// coordinates for the top left point, a width and a height

#ifndef CAMERA_H
#define CAMERA_H

#include <cstdint>
#include <climits>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "log.hpp"
#include "world/gameWorld.hpp"

namespace mc2d {


        class Camera {
        public:
                Camera(float xPos, float yPos, float zoom, uint8_t width, uint8_t height);
                ~Camera() = default;

                inline void             setPos(glm::vec3& pos)                  { m_pos = pos;          m_hasChanged = true; }
                inline void             setZoom(float zoom)                     { m_zoom = zoom;        m_hasChanged = true; }
                void                    setWidth(uint8_t width);
                void                    setHeight(uint8_t height);
                inline void             setHasChanged(bool hasChanged)          { m_hasChanged = hasChanged; }


                inline const glm::vec3& getPos() const                          { return m_pos; }
                inline float            getZoom() const                         { return m_zoom; }
                inline uint8_t          getWidth() const                        { return m_width; }
                inline uint8_t          getHeight() const                       { return m_height; }
                inline bool             hasChanged() const                      { return m_hasChanged; }

                glm::vec2               windowToWorldCoord(float x, float y, float windowWidth, float windowHeight) const;

                glm::mat4               getViewMatrix() const;
                void                    centerOnPoint(const glm::vec3& point);
                void                    updatePos(float x, float y)     { m_pos.x += x; m_pos.y += y; m_hasChanged = true; }
        
        private:
                static constexpr uint8_t        MIN_WIDTH = 8;  // Minimum width for a camera (measured in blocks)
                static constexpr uint8_t        MIN_HEIGHT = 8; // Minimum height for a camera (measured in blocks)

                glm::vec3                       m_pos;          // The camera position in world space
                float                           m_zoom;
                uint8_t                         m_width;        // The camera width measured in blocks
                uint8_t                         m_height;       // The camera height measured in blocks
                bool                            m_hasChanged;   // If true then visible blocks vertices must be recomputed during rendering
        };

}

#endif // CAMERA_H
