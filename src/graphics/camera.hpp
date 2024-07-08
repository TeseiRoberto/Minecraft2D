
// Contains definition of the Camera struct, the camera is simply a rectangle which has
// coordinates for the top left point, a width and a height

#ifndef CAMERA_H
#define CAMERA_H

namespace mc2d {

        struct Camera {
                float xPos      = 0.0f;                         // The top left x coordinate of the camera (in world space)
                float ypos      = 0.0f;                         // The top left y coordinate of the camera (in world space)

                uint32_t width   = 720;
                uint32_t height  = 480;
        };

}

#endif // CAMERA_H
