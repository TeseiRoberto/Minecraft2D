
cmake_minimum_required(VERSION 3.20)

project(Minecraft2D VERSION 1.0)

#add_compile_options("-ggdb")

set (SRCS
        libs/glad/src/glad.c
        libs/stbImage/stb_image.c

        src/main.cpp
        src/game.cpp
        src/block.cpp
        src/chunk.cpp

        src/graphics/shader.cpp
        src/graphics/renderer.cpp
        src/graphics/camera.cpp
        src/graphics/tileset.cpp
)

add_executable(minecraft2D ${SRCS})

target_include_directories(minecraft2D PRIVATE src/ libs/glad/include/ libs/stbImage libs/glm/)
target_link_libraries(minecraft2D glfw)
