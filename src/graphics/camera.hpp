
// Contains definition of the Camera class, the camera is simply a rectangle which has
// coordinates for the top left point, a width and a height

#ifndef CAMERA_H
#define CAMERA_H

#include <cstdint>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mc2d {

        class Camera {
        public:
                Camera(float xPos, float yPos, float zoom, uint8_t width, uint8_t height);
                ~Camera() = default;

                inline void             setPos(glm::vec3& pos)          { m_pos = pos;          m_hasChanged = true; }
                inline void             setZoom(float zoom)             { m_zoom = zoom;        m_hasChanged = true; }
                inline void             setWidth(uint8_t width)         { m_width = width;      m_hasChanged = true; }
                inline void             setHeight(uint8_t height)       { m_height = height;    m_hasChanged = true; }
                inline void             setHasChanged(bool changed)     { m_hasChanged = false; }

                inline const glm::vec3& getPos() const                  { return m_pos; }
                inline float            getZoom() const                 { return m_zoom; }
                inline uint8_t          getWidth() const                { return m_width; }
                inline uint8_t          getHeight() const               { return m_height; }
                inline bool             hasChanged() const              { return m_hasChanged; }

                glm::mat4               getViewMatrix() const;
                void                    updatePos(float x, float y)             { m_pos.x += x; m_pos.y += y; m_hasChanged = true;}

        private:
                glm::vec3       m_pos;          // The camera position in world space
                float           m_zoom;
                uint8_t         m_width;        // The camera width measured in blocks
                uint8_t         m_height;       // The camera height measured in blocks
                
                bool            m_hasChanged;   // Indicates if the camera position/size has changed
        };

}

#endif // CAMERA_H
