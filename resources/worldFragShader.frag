#version 330 core

in vec2 uv;
in float tileId;

uniform sampler2DArray grassTexture;

out vec4 outColor;

void main()
{
        outColor = texture(grassTexture, vec3(uv, tileId));
}
